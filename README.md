# MGO-Ordering-Related-Concurrency-Bugs: Artifact README

## Part A: Getting Started Guide

### 1. Artifact Description

This artifact provides the **MGO (Multi-Granularity Ordering)** dataset — a benchmark
of 58 real-world ordering-related concurrency bugs collected from open-source systems
(Linux Kernel, MySQL, MariaDB, DPDK, Open MPI, JDK, Xen, Ceph, Crossbeam, etc.).

The dataset is organized into a three-layer taxonomy based on the triggering level of
concurrency bugs:

| Layer | Name | Count | Description |
|-------|------|-------|-------------|
| Layer1 | Micro-Instruction Level | 32 | Pure hardware-sensitive reorderings (Store-Store, Store-Load) in lock-free data structures (spinlocks, ring buffers, etc.) |
| Layer2 | Resource Lifecycle Level | 24 | Hardware-agnostic and OS-level reorderings, such as Use-After-Free and kernel panics driven by thread scheduling or interrupts |
| Layer3 | Semantic Logic Level | 2 | Complex state machine or protocol disruptions in distributed systems and networking frameworks |

The artifact also includes automated evaluation pipelines for four verification tools:
- **CBMC** (Bounded Model Checker) — static formal verification under TSO/PSO memory models
- **GenMC** (Stateless Model Checker) — runtime verification under Relaxed/SC memory models
- **Nidhugg** (Stateless Model Checker) — runtime verification under SC/PSO memory models
- **IRhunter** (Dynamic Binary Instrumentation) — predictive detection of instruction reordering

### 2. Installation

#### 2.1 Extract the Artifact

```bash
unzip MGO-dataset.zip
cd Ordering-Related-Concurrency-Bugs-Dataset
```

#### 2.2 Install Required Tools

The following tools must be available on your system:

**CBMC (v5.12):**
```bash
sudo apt install cbmc
# Verify: cbmc --version
```

**GenMC (v0.9):**
```bash
git clone https://github.com/MPI-SWS/genmc.git
cd genmc && mkdir build && cd build
cmake .. && make -j$(nproc)
export PATH=$(pwd):$PATH
# Verify: genmc --version
```

**Nidhugg (v0.4):**
```bash
sudo apt install nidhugg
# Verify: nidhugg --version
```

**Clang 11.0.1 (for generating LLVM IR):**
```bash
sudo apt install clang-11
# Verify: clang --version
```

**IRhunter (Docker-based):**

IRhunter consists of two components:
1. **UFO** (dynamic instrumentation engine) — runs inside Docker
2. **reorder-main** (reordering prediction engine) — Maven-based Java project, included under `Evaluation/IRhunter/`

```bash
# Step 1: Pull the UFO Docker image (~5 GB)
docker pull 4ndychin/llvm-ufo

# Step 2: Start a container from the image
docker run -d --name irhunter-ufo 4ndychin/llvm-ufo tail -f /dev/null

# Step 3: Set the container ID for scripts to use
export IRHUNTER_CONTAINER_ID=$(docker ps -q --filter name=irhunter-ufo)

# Step 4: Build the reorder-main Maven project
cd Evaluation/IRhunter
mvn compile
```

### 3. Smoke Test

Run these commands to verify the artifact is intact and tools are operational:

```bash
# 1. Verify dataset integrity
echo "=== Dataset File Count ==="
echo "Layer1: $(ls Layer1/*.c 2>/dev/null | wc -l) files (expected: 32)"
echo "Layer2: $(ls Layer2/*.c 2>/dev/null | wc -l) files (expected: 24)"
echo "Layer3: $(ls Layer3/*.c 2>/dev/null | wc -l) files (expected: 2)"

# 2. Verify CBMC works
echo "=== CBMC Smoke Test ==="
cbmc --version

# 3. Verify GenMC works
echo "=== GenMC Smoke Test ==="
genmc --version

# 4. Verify Nidhugg works
echo "=== Nidhugg Smoke Test ==="
nidhugg --version

# 5. Verify evaluation scripts are executable
echo "=== Script Check ==="
ls Evaluation/CBMC/Layer1/run_cbmc.sh
ls Evaluation/GENMC\&Nidhugg/run_nidhugg.sh
ls Evaluation/GENMC\&Nidhugg/Layer3/run_genmc_eval.sh
ls Evaluation/IRhunter/loop_irhunter.sh
```

**Expected output:** All file counts match, all tools print version information,
and all script paths exist.

---

## Part B: Step-by-Step Reproduction Instructions

### Overview of Paper Claims Supported by This Artifact

| Claim | Supported? | How to Verify |
|-------|------------|---------------|
| MGO taxonomy classifies 58 real-world ordering-related concurrency bugs into 3 layers | Yes | Inspect files in Layer1/, Layer2/, Layer3/ |
| CBMC can detect WMM-specific bugs (TSO vs PSO differential analysis) | Yes | Run CBMC evaluation and check CSV reports |
| GenMC can detect WMM-specific bugs (Relaxed vs SC differential analysis) | Yes | Run GenMC evaluation and check CSV reports |
| Nidhugg can detect WMM-specific bugs (SC vs PSO differential analysis) | Yes | Run Nidhugg evaluation and check CSV reports |
| IRhunter can predictively detect instruction reordering vulnerabilities | Yes | Run IRhunter on individual bug samples |
| Bug provenance and real-world impact | Yes | See DATASET.md for source links and dates |

**Claims NOT supported by this artifact (and why):**
- Performance/benchmarking comparisons between tools: The provided scripts use
  conservative timeouts (10s for CBMC, 5s for Nidhugg) intended for verification,
  not performance measurement.
- Soundness/completeness proofs of individual tools: These belong to each tool's
  own research papers, not this dataset paper.
- Generalization to all possible WMM bugs: The dataset covers 58 representative
  samples but is not exhaustive.

### 1. Reproducing CBMC Results

CBMC performs bounded model checking under TSO and PSO memory models. A bug is
classified as a "WMM Bug" if it is Safe under TSO (stronger model) but triggers a
verification failure under PSO (weaker model, allowing Store-Load reordering).

**Run:**
```bash
cd Evaluation/CBMC/Layer1
bash run_cbmc.sh
```

**Expected output:**
- Consoles shows each file being tested with TSO/PSO results
- `cbmc_detailed.log` contains full CBMC output for each test case
- `cbmc_summary_report.csv` contains the summary table

**Interpret the CSV:**
- `WMM Bug`: TSO=Safe, PSO=Bug Detected → the bug is specific to weak memory
- `Logic Bug`: Both TSO and PSO detect a bug → a plain concurrency bug, not WMM-specific
- `No Bug`: Both models report Safe
- `Error/Timeout`: The sample uses features unsupported by CBMC (e.g., pointer-intensive code)

**Repeat for Layer2 and Layer3:**
```bash
cd Evaluation/CBMC/Layer2 && bash run_cbmc.sh
cd Evaluation/CBMC/Layer3 && bash run_cbmc.sh
```

### 2. Reproducing Nidhugg Results

Nidhugg performs stateless model checking under SC and PSO memory models using
LLVM IR (.ll files) as input.

**Step 1: Regenerate LLVM IR files (if needed):**
```bash
cd Evaluation/GENMC\&Nidhugg
bash generate_ll.sh
```

**Step 2: Run Nidhugg evaluation:**
```bash
cd Evaluation/GENMC\&Nidhugg
bash run_nidhugg.sh
```

**Expected output:**
- `nidhugg_summary_report.csv`: Folder, File_Name, SC_Model_Result, PSO_Model_Result
- `nidhugg_detailed.log`: Full trace output per test case

**Interpretation:**
- A bug is WMM-specific if SC=Safe and PSO=Bug Detected
- "Tool Error": the `.ll` file uses external functions unsupported by Nidhugg

### 3. Reproducing GenMC Results

GenMC performs verification under Relaxed (C11 memory_order_relaxed) and SC models.

**Run:**
```bash
cd Evaluation/GENMC\&Nidhugg/Layer3
bash run_genmc_eval.sh
```

**Expected output:**
- `genmc_evaluation_report2.csv`: Bug_ID, File_Name, Relaxed_Result, SC_Result, Conclusion

**Interpretation:**
- `Confirmed WMM Bug`: Relaxed=FAILED, SC=SUCCESS → WMM-specific
- `Logic Bug`: Both models fail → plain concurrency bug
- `False Negative`: Both models pass

### 4. Reproducing IRhunter Results

IRhunter performs predictive reordering detection in two stages:

**Stage 1 — Dynamic instrumentation (UFO in Docker):**
The `dynamic_run.sh` script copies a C source file into the UFO Docker container,
compiles it with a modified ThreadSanitizer-enabled clang, runs the binary with
UFO instrumentation enabled (`UFO_ON=1`), and collects execution traces to `./log/`.

**Stage 2 — Reordering prediction (reorder-main on host):**
The reorder-main Maven project reads the collected traces, runs a constraint-based
reordering analysis, and outputs predicted reordering vulnerabilities in SMT-LIB format.

**Full workflow:**
```bash
# 1. Ensure the UFO Docker container is running and ID is set
export IRHUNTER_CONTAINER_ID=$(docker ps -q --filter name=irhunter-ufo)

# 2. Enter the IRhunter evaluation directory
cd Evaluation/IRhunter

# 3. Run dynamic data capture (compiles, instruments, and collects traces)
bash dynamic_run.sh ../../Layer1/21-crossbeam.c

# 4. Run the reordering prediction engine
mvn exec:java -Dexec.mainClass="tju.edu.cn.reorder.ReorderMain"

# Or use the loop script to automate repeated attempts:
bash loop_irhunter.sh ../../Layer1/21-crossbeam.c
```

The `loop_irhunter.sh` script automates Stage 1 + Stage 2 with up to 10 attempts,
since concurrency bugs may not manifest on every single run. It re-executes both
stages until the target pattern is detected or the max attempts are exhausted.

**Pre-computed results:**
The `Evaluation/IRhunter/` directory contains detection reports for 18 bug samples
(e.g., `1-Linuxeasy`, `2-DPDK`, `4-MySQL`, etc.). Each report file contains the
raw constraint sets in SMT-LIB format produced by IRhunter's reordering analysis.
Reviewers can directly inspect these reports to verify the paper's IRhunter findings
without re-running the full toolchain.

### 5. Data Provenance

Each bug sample's real-world origin is documented in `DATASET.md` with:
- **Source**: URL to the original bug report, commit, or CVE entry
- **Target System**: The affected software project
- **Registered/Resolved Dates**: When the bug was reported and fixed

The dataset was collected from public sources (Linux Kernel Git, MySQL Bug Tracker,
GitHub Issues, CVE Database, etc.) under their respective open-source licenses.

### 6. Dataset Structure

```
Ordering-Related-Concurrency-Bugs-Dataset/
├── README.md              # This file (artifact documentation)
├── REQUIREMENTS           # Hardware/software requirements and tool versions
├── STATUS                 # Badge application and justification
├── LICENSE                # CC-BY-4.0
├── ABSTRACT.md            # Submission abstract
├── DATASET.md             # Detailed dataset description and provenance table
├── Layer1/                # 32 Micro-Instruction Level bug samples (.c, .cpp)
├── Layer2/                # 24 Resource Lifecycle Level bug samples (.c)
├── Layer3/                # 2 Semantic Logic Level bug samples (.c)
└── Evaluation/
    ├── CBMC/
    │   ├── Layer1/        # CBMC-formatted source + run_cbmc.sh + results
    │   ├── Layer2/        # CBMC-formatted source + run_cbmc.sh + results
    │   └── Layer3/        # CBMC-formatted source + run_cbmc.sh + results
    ├── GENMC&Nidhugg/
    │   ├── Layer1/        # GenMC .ll files for Nidhugg
    │   ├── Layer2/        # GenMC .ll files for Nidhugg
    │   ├── Layer3/        # GenMC source (.c) and .ll files + run_genmc_eval.sh
    │   ├── run_nidhugg.sh # Nidhugg batch evaluation script
    │   ├── generate_ll.sh # Script to regenerate .ll files from C sources
    │   ├── nidhugg_detailed.log
    │   └── nidhugg_summary_report.csv
    └── IRhunter/
        ├── dynamic_run.sh       # Docker orchestration script
        ├── loop_irhunter.sh     # Batch test loop script
        ├── pom.xml              # Maven project config
        ├── config.properties    # Prediction engine config
        ├── src/                 # Java source code (reorder-main)
        └── <bug-report-files>   # 18 IRhunter detection reports
```

### 7. Running Custom Experiments

To apply a new verification tool to this dataset:

1. Adapt the bug source files from `Layer1/`, `Layer2/`, `Layer3/` to your tool's input format
2. Create a directory under `Evaluation/YourToolName/`
3. Write a batch script following the pattern in `run_cbmc.sh` or `run_nidhugg.sh`
4. Compare your tool's classification against the ground truth in `DATASET.md`

The three-layer taxonomy is tool-agnostic: any verification tool that operates on
the concept of memory model strength can be evaluated using this benchmark.
