//https://github.com/MariaDB/server/pull/1952/commits
//https://github.com/MariaDB/server/pull/1951
// MariaDB server crashes on ARM (weak memory model architectures)
// This occurs during concurrent execution of
// l_find to load node->key and add_to_purgatory to store node->key = NULL.
// l_find then uses key (which is NULL at that point), passing it to the comparison function.
// The specific issue is out-of-order execution on weak memory model architectures. There are two possible reorderings that need to be prevented.
//
// a) Because l_find places no barrier between optimistically reading the key field lf_hash.cc#L117 and validating the link lf_hash.cc#L124,
// the processor can reorder the load operations to occur after the while loop.
//
// In this scenario, a concurrent thread executing add_to_purgatory on the same node can be scheduled to store the key field lf_alloc-pin.c#L253 as NULL before the key is loaded in l_find.

#include <stdio.h>
#include <stdlib.h>
#include <stdatomic.h>
#include <pthread.h>
#include <string.h>
#include <stdint.h>

typedef unsigned int uint32;
typedef unsigned char uchar;    /* Short for unsigned char */
typedef unsigned long intptr;

/*
  the last bit in LF_SLIST::link is a "deleted" flag.
  the helper macros below convert it to a pure pointer or a pure flag
*/
#define PTR(V)      (LF_SLIST *)((V) & (~(intptr)1))

typedef struct LF_SLIST {
    intptr link;   /* a pointer to the next element in a list and a flag */
    uint32 hashnr;         // Hash value of the node
    const uchar *key;      // Pointer to the node's key
    size_t keylen;         // Length of the key
} LF_SLIST;

typedef struct {
    LF_SLIST **prev; // Pointer to the previous node
    LF_SLIST *curr;  // Current node
    LF_SLIST *next;  // Next node
} CURSOR;

#define LF_PINBOX_PINS 4
#define CPU_LEVEL1_DCACHE_LINESIZE 64
#define LF_DYNARRAY_LEVELS       4

typedef void lf_pinbox_free_func(void *, void *, void *);

typedef struct {
    void *volatile level[LF_DYNARRAY_LEVELS];
    uint size_of_element;
} LF_DYNARRAY;

typedef struct {
    LF_DYNARRAY pinarray;
    lf_pinbox_free_func *free_func;
    void *free_func_arg;
    uint free_ptr_offset;
    uint32 volatile pinstack_top_ver;         /* this is a versioned pointer */
    uint32 volatile pins_in_array;            /* number of elements in array */
} LF_PINBOX;

typedef struct {
    void *volatile pin[LF_PINBOX_PINS];
    LF_PINBOX *pinbox;
    void *purgatory;
    uint32 purgatory_count;
    uint32 volatile link;
    /* avoid false sharing */
    char pad[CPU_LEVEL1_DCACHE_LINESIZE];
} LF_PINS;

#define compile_time_assert(X)                                  \
  do                                                            \
  {                                                             \
    typedef char compile_time_assert[(X) ? 1 : -1] __attribute__((unused)); \
  } while(0)
#define my_atomic_storeptr(P, D) __atomic_store_n((P), (D), __ATOMIC_SEQ_CST)

/* compile-time assert to make sure we have enough pins.  */
#define lf_pin(PINS, PIN, ADDR)                                \
  do {                                                          \
    compile_time_assert(PIN < LF_PINBOX_PINS);                  \
    my_atomic_storeptr(&(PINS)->pin[PIN], (ADDR));              \
  } while(0)
#define my_atomic_casptr(P, E, D) \
  __atomic_compare_exchange_n((P), (E), (D), 0, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)

static int l_find(LF_SLIST **head, const uchar *key, size_t keylen, CURSOR *cursor, LF_PINS *pins) {
    uint32 cur_hashnr;
    const uchar *cur_key;
    size_t cur_keylen;
    intptr link;

    for (;;) {
        if (!cursor->curr)
            return 0; // End of list

        cur_hashnr = cursor->curr->hashnr;
        cur_keylen = cursor->curr->keylen;

        // Optimistically read key; this is the critical part for concurrent access
        cur_key = cursor->curr->key;

        // Traverse to next node, considering memory alignment and concurrency safety
        do {
            link = cursor->curr->link;
            cursor->next = PTR(link);

            // Use lf_pin to "pin" the next node, avoiding concurrent deletion
            lf_pin(pins, 0, cursor->next);
        } while (link != cursor->curr->link); // Check if link was modified during read, ensuring concurrency consistency

        // Processing logic for cur_key, e.g., comparison with incoming key, can be added here

        cursor->curr = cursor->next;
        lf_pin(pins, 1, cursor->curr);
    }
}

#define add_to_purgatory(PINS, ADDR)                                    \
  do                                                                    \
  {                                                                     \
    *(void **)((char *)(ADDR)+(PINS)->pinbox->free_ptr_offset)=         \
      (PINS)->purgatory;                                                \
    (PINS)->purgatory= (ADDR);                                          \
    (PINS)->purgatory_count++;                                          \
  } while (0)

#define LF_PURGATORY_SIZE 100

void lf_pinbox_real_free(LF_PINS* pins) {
    // Assuming purgatory is a singly-linked list of nodes to be freed.
    void * current = pins->purgatory;
    void *next;

    while (current != NULL) {
        next = current->next; // Save next node.
        free(current); // Free the current node.
        current = next; // Move to next node.
    }

    // After freeing, reset the purgatory list and count.
    pins->purgatory = NULL;
    pins->purgatory_count = 0;
}

static int l_delete(LF_SLIST **head, const uchar * *cs, uint32 hashnr,
                    const uchar *key, uint keylen, LF_PINS *pins) {
    CURSOR cursor;
    int res;

    for (;;) {
        if (!l_find(head, key, keylen, &cursor, pins)) {
            res = 1; // not found
            break;
        } else {
            // Ensure pointer is correctly cast to integer before compare-and-swap (CAS) and setting delete mark
            void *expected = cursor.next;
            void *newValue = (void *)((intptr_t)cursor.next | 1);
            if (my_atomic_casptr(&(cursor.curr->link), &expected, newValue)) {
                // Attempt physical deletion from the list
                if (my_atomic_casptr(cursor.prev, &cursor.curr, cursor.next)) {
                    add_to_purgatory(pins, cursor.curr); // Assuming addr is equivalent to cursor.curr
                    // Check if cleanup needs to be performed
                    if (pins->purgatory_count % LF_PURGATORY_SIZE == 0)
                        lf_pinbox_real_free(pins);
                }
                res = 0; // Successfully deleted
                break;
            }
        }
    }
    return res;
}