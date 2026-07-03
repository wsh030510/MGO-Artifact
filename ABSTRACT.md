# Artifact Abstract

## Paper Title

Understanding Real-World Ordering-Related Concurrency Bugs under Weak-Memory and Cross-Architecture Settings

## Link to the Accepted Paper

https://ase2026-industry.hotcrp.com/paper/129

## Purpose

This artifact provides the MGO (Multi-Granularity Ordering) dataset — a benchmark of
58 real-world ordering-related concurrency bugs collected from open-source systems
including the Linux Kernel, MySQL, MariaDB, DPDK, Open MPI, JDK, Xen, and others.
The dataset is organized into a three-layer taxonomy:

- Layer 1 (Micro-Instruction Level): 32 samples of pure hardware-sensitive reorderings
  (Store-Store, Store-Load) in lock-free data structures.
- Layer 2 (Resource Lifecycle Level): 24 samples of OS-level races manifesting as
  Use-After-Free or kernel panics.
- Layer 3 (Semantic Logic Level): 2 samples of complex state machine disruptions in
  distributed systems and networking frameworks.

The artifact includes automated evaluation pipelines for four verification tools —
CBMC, GenMC, Nidhugg, and IRhunter — with scripts, pre-computed results, and
detailed reproduction instructions.

## Badge

The authors are applying for the following badges:

- **Available**: The artifact is archived on Zenodo with a permanent DOI and released
  under the CC-BY-4.0 open-source license.
- **Reusable**: The artifact is carefully structured with a clear three-layer taxonomy,
  automated evaluation scripts, pre-computed CSV results for cross-validation,
  containerized tool dependencies via Docker, and comprehensive documentation to
  facilitate reuse and repurposing by other researchers.

## Technology Skills Assumed by the Reviewer

- Basic familiarity with Linux command line and shell scripting
- Basic understanding of concurrency bugs and weak memory models
- Experience with Docker (for running IRhunter components)
- No prior experience with CBMC, GenMC, Nidhugg, or IRhunter is required;
  the provided scripts automate their usage.

## Hardware Requirements

- Architecture: x86_64
- OS: Linux (tested on Ubuntu 22.04)
- Storage: ~500 MB for the artifact; additional ~5 GB if building the IRhunter Docker image
- No GPU or other special hardware required

## Provenance

The artifact is available at:
- Zenodo: [DOI to be provided upon acceptance]
- Software Heritage: [to be provided]

## Instructions

### Accessing the Artifact

1. Download the artifact from Zenodo (DOI to be provided).
2. Extract the archive: `unzip MGO-dataset.zip`
3. Enter the directory: `cd Ordering-Related-Concurrency-Bugs-Dataset`

### Tools Required

The following tools are pre-installed on the evaluation system:
- CBMC 5.12 (Bounded Model Checker)
- GenMC v0.9 (Stateless Model Checker)
- Nidhugg 0.4 (Stateless Model Checker)
- Clang 11.0.1 (LLVM IR generator)
- Docker (for IRhunter/UFO dynamic instrumentation engine)
- Maven + JDK 8 (for IRhunter reorder-main prediction engine)

### Getting Started (Smoke Test)

Verify the artifact integrity:
```
$ ls Layer1/*.c | wc -l
32
$ ls Layer2/*.c | wc -l
24
$ ls Layer3/*.c | wc -l
2
```

### Step-by-Step Reproduction Instructions

See the README.md file for detailed reproduction instructions for each tool
(CBMC, GenMC, Nidhugg, IRhunter).

### Dataset Schema

Each bug sample in Layer1/Layer2/Layer3 is a self-contained C/C++ source file.
The dataset metadata (bug ID, source, target system, type, dates) is documented
in DATASET.md. Evaluation results are stored as CSV files with the following schema:

- CBMC: File_Name, TSO_Result, PSO_Result, Conclusion
- Nidhugg: Folder, File_Name, SC_Model_Result, PSO_Model_Result
- GenMC: Bug_ID, File_Name, Relaxed_Result, SC_Result, Conclusion
- IRhunter: RawReorder constraint sets in SMT-LIB format
