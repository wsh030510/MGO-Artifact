/*
 *  MySQL Bug #74832
 *https://bugs.mysql.com/bug.php?id=74832
 */

#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdatomic.h>
#include <unistd.h>

/* ── Simulate core fields of MySQL mutex ─────────────────────── */
typedef struct {
    atomic_int lock_word;   /* 0=free, 1=locked */
    int        data;        /* Shared data protected by lock */
    int        data_ready;  /* Flag: whether data has been initialized */
} inno_mutex_t;

inno_mutex_t g_mutex = { .lock_word = 0, .data = 0, .data_ready = 0 };

/* ── Simulate buggy UT_RELAX_CPU (original implementation on non-x86 platforms) ───── */
/* Original code used compare_and_swap, producing a full memory barrier        */
static inline void ut_relax_cpu_buggy(void)
{
    /* Simulate: os_compare_and_swap_lint(&volatile_var, 0, 1)
     * On POWER this generates sync + ldarx + stdcx + isync
     * i.e., a full SEQ_CST barrier — very expensive in a spin loop    */
    volatile int volatile_var = 0;
    (void)__sync_val_compare_and_swap(&volatile_var, 0, 1);
    /* This accidental strong barrier prevents the following load from being reordered before it */
}

/* ── Simulate fixed UT_RELAX_CPU (compiler barrier only) ─────────── */
static inline void ut_relax_cpu_fixed(void)
{
    /* Fix: only compiler barrier, no hardware memory barrier instruction        */
    __asm__ __volatile__ ("":::"memory");
    /* At this point CPU can still reorder: data_ready load may occur
     * before lock_word load — weak memory vulnerability exposed!        */
}

/* ── Spin lock (simulates mutex_spin_wait) ───────────────────── */
static void mutex_lock(inno_mutex_t *m, int use_buggy_relax)
{
    int expected;
    /* Attempt to acquire lock */
    while (1) {
        expected = 0;
        if (atomic_compare_exchange_weak_explicit(
                &m->lock_word, &expected, 1,
                memory_order_acquire, memory_order_relaxed))
            break;  /* Acquired successfully */

        /* Spin wait: this is the core of the bug */
        for (int i = 0; i < 50; i++) {
            if (use_buggy_relax)
                ut_relax_cpu_buggy();   /* Accidentally generates strong barrier */
            else
                ut_relax_cpu_fixed();   /* Compiler barrier only   */
        }
    }
}

static void mutex_unlock(inno_mutex_t *m)
{
    atomic_store_explicit(&m->lock_word, 0, memory_order_release);
}

/* ── Test race scenario ─────────────────────────────────────── */
#define ITERATIONS 2000000

volatile int bug_detected = 0;
volatile int start_flag   = 0;

/*
 * Writer thread:
 *   Writes data, then sets data_ready, finally releases lock.
 *   On weak memory models, if spin loop lacks sufficient barriers,
 *   Reader may see data_ready=1 but data is still old value.
 */
void *thread_writer(void *arg)
{
    int use_buggy = *(int *)arg;

    while (!start_flag);

    for (int i = 0; i < ITERATIONS && !bug_detected; i++) {
        mutex_lock(&g_mutex, use_buggy);

        /* 1. Write data */
        g_mutex.data = i + 1;

        /* 2. Missing write barrier (simulating bug)
         *    On weak memory, data_ready write may become visible
         *    to other CPUs before data write           */
        g_mutex.data_ready = 1;

        mutex_unlock(&g_mutex);
    }
    return NULL;
}

/*
 * Reader thread:
 *   Spins outside lock waiting for data_ready, then reads data.
 *   Simulates MySQL scenario of reading shared state within spin loop.
 */
void *thread_reader(void *arg)
{
    int use_buggy = *(int *)arg;
    long ooo_count = 0;

    while (!start_flag);

    for (int i = 0; i < ITERATIONS && !bug_detected; i++) {
        /* Spin waiting for data_ready, using buggy/fixed relax   */
        int ready = 0;
        while (!ready) {
            if (use_buggy)
                ut_relax_cpu_buggy();
            else
                ut_relax_cpu_fixed();

            /* Read data_ready (no acquire semantics — simulating bug) */
            ready = atomic_load_explicit(&g_mutex.data_ready,
                                         memory_order_relaxed);
        }

        /* Read data: if reordering occurred, data may still be 0 */
        int val = g_mutex.data;

        /* Clear data_ready (requires lock; simplified to direct clear)  */
        atomic_store_explicit(&g_mutex.data_ready, 0,
                              memory_order_relaxed);

        if (val == 0 && ready == 1) {
            ooo_count++;
            if (ooo_count == 1) {
                printf("[!] OOO Bug Detected at iter %d: "
                       "data_ready=1 but data=0!\n", i);
                printf("    Cause: Load reordering in spin loop "
                       "(missing acquire barrier)\n");
                bug_detected = 1;
            }
        }
    }

    if (!bug_detected)
        printf("[*] Reader finished. OOO events: %ld\n", ooo_count);

    return NULL;
}

/* ── Main function: compare buggy and fixed behavior ─────────────────── */
int main(void)
{
    pthread_t tw, tr;
    int use_buggy;

    printf("=================================================\n");
    printf("MySQL UT_RELAX_CPU Weak Memory Bug Reproduction\n");
    printf("Bug #74832: UT_DELAY MISSING COMPILER BARRIER\n");
    printf("=================================================\n\n");

    /* ── Test1: Using buggy UT_RELAX_CPU ─────────────── */
    printf("[Test 1] Buggy UT_RELAX_CPU (with accidental full barrier)\n");
    printf("         On ARM/POWER: each spin iteration = sync+isync\n");
    g_mutex.lock_word  = 0;
    g_mutex.data       = 0;
    g_mutex.data_ready = 0;
    bug_detected       = 0;
    start_flag         = 0;

    use_buggy = 1;
    pthread_create(&tw, NULL, thread_writer, &use_buggy);
    pthread_create(&tr, NULL, thread_reader, &use_buggy);
    start_flag = 1;
    pthread_join(tw, NULL);
    pthread_join(tr, NULL);

    if (!bug_detected)
        printf("[OK] No OOO detected (accidental barriers masked the bug)\n\n");

    /* ── Test2: Using fixed UT_RELAX_CPU ──────────────── */
    printf("[Test 2] Fixed UT_RELAX_CPU (compiler barrier only)\n");
    printf("         On ARM/POWER: no hardware fence, OOO may occur\n");
    g_mutex.lock_word  = 0;
    g_mutex.data       = 0;
    g_mutex.data_ready = 0;
    bug_detected       = 0;
    start_flag         = 0;

    use_buggy = 0;
    pthread_create(&tw, NULL, thread_writer, &use_buggy);
    pthread_create(&tr, NULL, thread_reader, &use_buggy);
    start_flag = 1;
    pthread_join(tw, NULL);
    pthread_join(tr, NULL);

    if (!bug_detected)
        printf("[OK] No OOO detected on this run (x86 TSO hides it)\n");

    printf("\n[Summary]\n");
    printf("  On x86 (TSO): both tests pass, bug is hidden\n");
    printf("  On ARM/POWER: Test1 may hide bug via accidental barrier,\n");
    printf("                Test2 exposes OOO reordering\n");
    printf("  Fix: add memory_order_acquire to data_ready load\n");

    return 0;
}