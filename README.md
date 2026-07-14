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
tar -xzf MGO-dataset.tar.gz
cd Ordering-Related-Concurrency-Bugs-Dataset
```

#### 2.2 Docker Image (CBMC, GenMC, Nidhugg)

The recommended way to run CBMC, GenMC, and Nidhugg is via Docker.
The pre-built Docker image is available on Docker Hub.

**Option A: Pull from Docker Hub (recommended)**

```bash
docker pull wangshaohao/mgo-ae:latest
```

**Option B: Build from source (if Docker Hub is unavailable)**

```bash
docker build -t mgo-ae .
```

The build takes 10-20 minutes.

**Prerequisites:** Docker 20.10+ on x86_64 Linux.

The image (~830 MB) contains:
- **CBMC 5.12** — installed via Debian package
- **GenMC v0.9** — compiled from source with LLVM 11
- **Nidhugg 0.4** — compiled from source with LLVM 11
- All 58 benchmark files and evaluation scripts under `/benchmarks/`

Verify:

```bash
docker run --rm wangshaohao/mgo-ae:latest
# Or if built from source: docker run --rm mgo-ae
```

Expected output: version information for all three tools plus benchmark listing.

#### 2.3 IRhunter (Separate Docker Setup)

IRhunter requires a pre-built Docker image (`4ndychin/llvm-ufo`) and Maven.
It is NOT included in the `mgo-ae` image.

```bash
# Pull the UFO Docker image (~5 GB)
docker pull 4ndychin/llvm-ufo

# Start a container
docker run -d --name irhunter-ufo 4ndychin/llvm-ufo tail -f /dev/null
export IRHUNTER_CONTAINER_ID=$(docker ps -q --filter name=irhunter-ufo)

# Build the reorder-main Maven project
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

# 2. Verify Docker image and tools
docker run --rm mgo-ae

# 3. Quick tool verification with a sample benchmark
docker run --rm mgo-ae bash -c "
  echo '=== CBMC ===' && cbmc --version &&
  echo '=== GenMC ===' && genmc --version &&
  echo '=== Nidhugg ===' && nidhugg --version
"

# 4. Verify evaluation scripts are executable
ls Evaluation/CBMC/Layer1/run_cbmc.sh
ls Evaluation/GENMC\&Nidhugg/run_nidhugg.sh
ls Evaluation/GENMC\&Nidhugg/Layer1/run_genmc_eval.sh
```

**Expected output:** All file counts match, all tools print version information,
and all script paths exist.

---

## Part B: Review Workflow

### Quick Start for Reviewers

*Estimated time: ~10 minutes (pull + evaluations). No manual tool installation required.*

---

#### Step 1: Pull the Docker Image

The CBMC, GenMC, and Nidhugg tools are pre-built into a single Docker image
hosted on Docker Hub:

```bash
docker pull wangshaohao/mgo-ae:latest
```

*If Docker Hub is inaccessible in your region, build the image from source instead:*
```bash
cd Ordering-Related-Concurrency-Bugs-Dataset
docker build -t mgo-ae .
```
*Then replace `wangshaohao/mgo-ae:latest` with `mgo-ae` in all commands below.*

**Verify the image is working:**

```bash
docker run --rm wangshaohao/mgo-ae:latest
```

Expected output: version banners for CBMC 5.12, GenMC v0.9, Nidhugg 0.4,
and a listing of benchmark directories inside the container.

---

#### Step 2: Reproduce CBMC Results

CBMC performs bounded model checking under TSO and PSO memory models.
Each test case is classified by comparing results across the two models.

**Run Layer 1 (32 cases):**
```bash
docker run --rm wangshaohao/mgo-ae:latest \
  bash -c "cd /benchmarks/CBMC/Layer1 && bash run_cbmc.sh"
```

**Run Layer 2 (24 cases):**
```bash
docker run --rm wangshaohao/mgo-ae:latest \
  bash -c "cd /benchmarks/CBMC/Layer2 && bash run_cbmc.sh"
```

**Run Layer 3 (2 cases):**
```bash
docker run --rm wangshaohao/mgo-ae:latest \
  bash -c "cd /benchmarks/CBMC/Layer3 && bash run_cbmc.sh"
```

Each run produces `cbmc_summary_report.csv` containing:
`File_Name, TSO_Result, PSO_Result, Conclusion`.

**Interpretation:**
- `WMM Bug` — TSO=Safe, PSO=Bug Detected → weak-memory-specific bug
- `Logic Bug` — both models detect a bug → plain concurrency bug
- `No Bug` — both models report Safe
- `Error/Timeout` — unsupported features (e.g., pointer-heavy code)

---

#### Step 3: Reproduce Nidhugg Results

Nidhugg performs stateless model checking under SC and PSO memory models
using pre-compiled LLVM IR (`.ll`) files.

```bash
docker run --rm wangshaohao/mgo-ae:latest \
  bash -c "cd /benchmarks/GENMC\&Nidhugg && bash run_nidhugg.sh"
```

**Output:** `nidhugg_summary_report.csv` with `Folder, File_Name, SC_Model_Result, PSO_Model_Result`.

**Interpretation:** A WMM-specific bug is detected when SC=Safe and PSO=Bug Detected.

*Note: If the pre-compiled `.ll` files are incompatible with the Docker environment,
regenerate them first:*
```bash
docker run --rm wangshaohao/mgo-ae:latest \
  bash -c "cd /benchmarks/GENMC\&Nidhugg && bash generate_ll.sh"
```

---

#### Step 4: Reproduce GenMC Results

GenMC performs verification under Relaxed (C11 `memory_order_relaxed`) and SC models.

**Layer 1 (32 cases):**
```bash
docker run --rm wangshaohao/mgo-ae:latest \
  bash -c "cd /benchmarks/GENMC\&Nidhugg/Layer1 && bash run_genmc_eval.sh"
```

**Layer 2 (24 cases):**
```bash
docker run --rm wangshaohao/mgo-ae:latest \
  bash -c "cd /benchmarks/GENMC\&Nidhugg/Layer2 && bash run_genmc_eval.sh"
```

**Layer 3 (4 cases):**
```bash
docker run --rm wangshaohao/mgo-ae:latest \
  bash -c "cd /benchmarks/GENMC\&Nidhugg/Layer3 && bash run_genmc_eval.sh"
```

**Output:** `genmc_evaluation_report.csv` with `Bug_ID, File_Name, Relaxed_Result, SC_Result, Conclusion`.

**Interpretation:**
- `Confirmed WMM Bug` — Relaxed=FAILED, SC=SUCCESS → WMM-specific
- `Logic Bug` — both models fail → plain concurrency bug
- `False Negative` — both models pass

---

#### Step 5: (Optional) Reproduce IRhunter Results

IRhunter uses a separate Docker image provided by its authors. Pre-computed
results for 18 bugs are available in `Evaluation/IRhunter/`.

**Setup:**
```bash
docker pull 4ndychin/llvm-ufo
docker run -d --name irhunter-ufo 4ndychin/llvm-ufo tail -f /dev/null
export IRHUNTER_CONTAINER_ID=$(docker ps -q --filter name=irhunter-ufo)
cd Evaluation/IRhunter && mvn compile
```

**Run a single test:**
```bash
bash loop_irhunter.sh ../../Layer1/21-crossbeam.c
```

The `loop_irhunter.sh` script automates the two-stage pipeline (UFO dynamic
instrumentation + reorder-main constraint analysis) with up to 10 retry
attempts, since concurrency bugs may not manifest on every execution.

---

#### Step 6: Verify Results

Compare the CSV outputs from each evaluation with the pre-computed results
included in the `Evaluation/` directory:

```bash
# Example for CBMC Layer 1
diff <(cat Evaluation/CBMC/Layer1/cbmc_summary_report.csv) \
     <(docker run --rm wangshaohao/mgo-ae:latest \
       cat /benchmarks/CBMC/Layer1/cbmc_summary_report.csv)
```

Reviewers can confirm this by running any of the above commands and comparing the output.

---

### Overview of Paper Claims Supported by This Artifact

| Claim | Supported? | How to Verify |
|-------|------------|---------------|
| MGO taxonomy classifies 58 real-world ordering-related concurrency bugs into 3 layers | Yes | Inspect files in Layer1/, Layer2/, Layer3/ |
| CBMC can detect WMM-specific bugs (TSO vs PSO differential analysis) | Yes | Run CBMC evaluation via Docker and check CSV reports |
| GenMC can detect WMM-specific bugs (Relaxed vs SC differential analysis) | Yes | Run GenMC evaluation via Docker and check CSV reports |
| Nidhugg can detect WMM-specific bugs (SC vs PSO differential analysis) | Yes | Run Nidhugg evaluation via Docker and check CSV reports |
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
docker run --rm mgo-ae bash -c "cd /benchmarks/CBMC/Layer1 && bash run_cbmc.sh"
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
docker run --rm mgo-ae bash -c "cd /benchmarks/CBMC/Layer2 && bash run_cbmc.sh"
docker run --rm mgo-ae bash -c "cd /benchmarks/CBMC/Layer3 && bash run_cbmc.sh"
```

### 2. Reproducing Nidhugg Results

Nidhugg performs stateless model checking under SC and PSO memory models using
LLVM IR (.ll files) as input.

**Step 1: Regenerate LLVM IR files (if needed):**
```bash
docker run --rm mgo-ae bash -c "cd /benchmarks/GENMC\&Nidhugg && bash generate_ll.sh"
```

**Step 2: Run Nidhugg evaluation:**
```bash
docker run --rm mgo-ae bash -c "cd /benchmarks/GENMC\&Nidhugg && bash run_nidhugg.sh"
```

**Expected output:**
- `nidhugg_summary_report.csv`: Folder, File_Name, SC_Model_Result, PSO_Model_Result
- `nidhugg_detailed.log`: Full trace output per test case

**Interpretation:**
- A bug is WMM-specific if SC=Safe and PSO=Bug Detected
- "Tool Error": the `.ll` file uses external functions unsupported by Nidhugg

### 3. Reproducing GenMC Results

GenMC performs verification under Relaxed (C11 memory_order_relaxed) and SC models.

**Run Layer1:**
```bash
docker run --rm mgo-ae bash -c "cd /benchmarks/GENMC\&Nidhugg/Layer1 && bash run_genmc_eval.sh"
```

**Run Layer2:**
```bash
docker run --rm mgo-ae bash -c "cd /benchmarks/GENMC\&Nidhugg/Layer2 && bash run_genmc_eval.sh"
```

**Run Layer3:**
```bash
docker run --rm mgo-ae bash -c "cd /benchmarks/GENMC\&Nidhugg/Layer3 && bash run_genmc_eval.sh"
```

**Expected output:**
- `genmc_evaluation_report.csv`: Bug_ID, File_Name, Relaxed_Result, SC_Result, Conclusion

**Interpretation:**
- `Confirmed WMM Bug`: Relaxed=FAILED, SC=SUCCESS → WMM-specific
- `Logic Bug`: Both models fail → plain concurrency bug
- `False Negative`: Both models pass

### 4. Reproducing IRhunter Results

IRhunter is a predictive reordering detection tool originally developed by its
authors. It consists of two components:

1. **UFO** — A modified LLVM/Clang 7.0 toolchain with ThreadSanitizer extensions
   for dynamic instrumentation. The authors distribute this as a Docker image.
2. **reorder-main** — A Java/Maven constraint-based reordering analysis engine
   that reads execution traces and predicts reordering vulnerabilities.

This artifact uses the UFO Docker image provided by the IRhunter authors,
combined with the reorder-main analysis engine included under `Evaluation/IRhunter/`.

#### 4.1 IRhunter Architecture

```
                               ┌─────────────────────────┐
                               │   UFO Docker Container   │
  Source File (.c)  ── ─►   │   (4ndychin/llvm-ufo)   │
                               │                          │
                               │  1. Modified clang       │
                               │     (-fsanitize=thread)  │
                               │  2. UFO-instrumented     │
                               │     binary execution     │
                               │  3. Trace collection     │
                               └──────────┬──────────────┘
                                          │
                                    Execution Traces
                                          │
                               ┌──────────▼──────────────┐
                               │   reorder-main (Host)    │
                               │                          │
                               │  Maven + Java 8          │
                               │  Constraint-based        │
                               │  reordering analysis     │
                               │                          │
                               │  Output: SMT-LIB         │
                               │  reordering reports      │
                               └─────────────────────────┘
```

#### 4.2 Install IRhunter (UFO Docker + Maven)

**Step 1: Pull the UFO Docker image from the IRhunter authors.**

The UFO Docker image is provided by the IRhunter authors at:
https://hub.docker.com/r/4ndychin/llvm-ufo

```bash
docker pull 4ndychin/llvm-ufo
```

Image details: ~5 GB, Ubuntu 16.04 base, LLVM 7.0 with modified ThreadSanitizer.

**Step 2: Start a UFO container.**

```bash
docker run -d --name irhunter-ufo 4ndychin/llvm-ufo tail -f /dev/null
```

**Step 3: Set the container ID for scripts.**

```bash
export IRHUNTER_CONTAINER_ID=$(docker ps -q --filter name=irhunter-ufo)
```

The `dynamic_run.sh` script reads `IRHUNTER_CONTAINER_ID` from the environment
to communicate with the UFO container (copy files in, run compilation/instrumentation,
retrieve traces).

**Step 4: Build the reorder-main Maven project.**

```bash
cd Evaluation/IRhunter
mvn compile
```

Requires: JDK 8 and Maven.

#### 4.3 Testing with dynamic_run.sh (Stage 1: UFO Instrumentation)

`dynamic_run.sh` is adapted from the IRhunter authors' original script.
It performs the following steps for a single bug sample:

1. Copies the C source file into the UFO Docker container
2. Compiles it with the modified clang (`-fsanitize=thread -g -O0`)
3. Runs the instrumented binary with UFO enabled (`UFO_ON=1 UFO_CALL=1`)
4. Collects execution traces back to `./log/<binary_name>/`

Usage:
```bash
cd Evaluation/IRhunter
bash dynamic_run.sh ../../Layer1/21-crossbeam.c
```

On success, traces are saved to `./log/21-crossbeam/` and `config.properties`
is automatically updated to point to the new trace directory.

#### 4.4 Testing with loop_irhunter.sh (Full Pipeline)

Since concurrency bugs do not always manifest on every execution,
`loop_irhunter.sh` wraps both Stage 1 (UFO) and Stage 2 (reorder-main)
into an automated retry loop (up to 10 attempts).

For each attempt it:
1. Runs `dynamic_run.sh` to collect a fresh set of execution traces
2. Runs `mvn exec:java` to perform reordering analysis on the traces
3. Checks if the expected pattern was detected; if so, exits successfully
4. Otherwise, retries

Usage:
```bash
cd Evaluation/IRhunter
bash loop_irhunter.sh ../../Layer1/21-crossbeam.c
```

**IMPORTANT:** Before running `loop_irhunter.sh`, edit the `SUCCESS_KEYWORD`
variable in the script (line 11) to match the expected output pattern for
the target bug sample. The keyword depends on what IRhunter should detect
for that particular bug.

#### 4.5 Interpreting IRhunter Output

When IRhunter detects a reordering vulnerability, it produces:
- Execution trace logs under `./log/<binary_name>/`
- SMT-LIB constraint files showing the specific instruction reordering

The reordering prediction indicates that under a weak memory model, two memory
accesses could be reordered by the CPU, potentially causing a concurrency bug
even in otherwise correctly synchronized code.

#### 4.6 Pre-Computed Results

The `Evaluation/IRhunter/` directory contains pre-computed detection reports for
18 bug samples from Layer1 (files named `1-Linuxeasy`, `2-DPDK`, `4-MySQL`, etc.).
Each report file contains the raw constraint sets in SMT-LIB format produced by
IRhunter's reordering analysis. Reviewers can directly inspect these reports to
verify the paper's IRhunter findings without re-running the full pipeline.

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
├── Dockerfile             # Docker build for CBMC + GenMC + Nidhugg
├── .dockerignore
├── docker-entrypoint.sh   # Docker container entrypoint
├── genmc-src/             # GenMC v0.9 source (built inside Docker)
├── nidhugg-src/           # Nidhugg 0.4 source (built inside Docker)
├── Layer1/                # 32 Micro-Instruction Level bug samples (.c, .cpp)
├── Layer2/                # 24 Resource Lifecycle Level bug samples (.c)
├── Layer3/                # 2 Semantic Logic Level bug samples (.c)
└── Evaluation/
    ├── CBMC/
    │   ├── Layer1/        # CBMC-formatted source + run_cbmc.sh + results
    │   ├── Layer2/        # CBMC-formatted source + run_cbmc.sh + results
    │   └── Layer3/        # CBMC-formatted source + run_cbmc.sh + results
    ├── GENMC&Nidhugg/
    │   ├── Layer1/        # GenMC .c + .ll files + run_genmc_eval.sh + results
    │   ├── Layer2/        # GenMC .c + .ll files + run_genmc_eval.sh + results
    │   ├── Layer3/        # GenMC .c + .ll files + run_genmc_eval.sh + results
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
