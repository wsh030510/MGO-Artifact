**MGO-Ordering-Related-Concurrency-Bugs**: A Benchmark of Multi-Granularity Ordering-Related-Concurrency-Bugs

This repository contains the dataset for our paper "Understanding Real-World Ordering-Related Concurrency Bugs under Weak-Memory and Cross-Architecture Settings". We propose a Multi-Granularity Ordering (MGO) taxonomy and collect 58 Ordering-Related-Concurrency-Bugs samples across different system abstraction layers.

📂 MGO Taxonomy Structure
Our dataset is strictly categorized into three layers based on the triggering level of the concurrency bugs:
* **[Layer1](./Layer1)**: Micro-Instruction Level (32 Samples). Focuses on pure hardware-sensitive reorderings (Store-Store, Store-Load) in fundamental lock-free data structures (e.g., Spinlocks, Ring Buffers).
* **[Layer2](./Layer2)**: Resource Lifecycle Level (24 Samples). Focuses on hardware-agnostic and OS-level reorderings, typically manifesting as Use-After-Free (UAF) or Kernel Panics driven by thread scheduling or interrupts (e.g., Linux Kernel, Drivers).
* **[Layer3](./Layer3)**: Semantic Logic Level (2 Samples). Focuses on complex state machine or protocol disruptions in high-performance distributed systems and networking frameworks (e.g., MySQL, DPDK, sofa-pbrpc).

🏷️ Label Definitions:
* **Bug Id**: Our own defined internal reference number for the bug (e.g., L1-01).
* **Source**: The source of the vulnerability or the submission number of the vulnerability.
* **Target System**: The software project or framework where the bug is located (e.g., MySQL, Linux Kernel, DPDK).
* **Type**: The specific manifestation or category of the concurrency error (e.g., Unsafe Publication, Store-Load reordering).
* **Registered**: The date the bug was initially reported or raised by the community.
* **Resolved**: The date the bug was officially fixed or closed by the developers.

**Layer 1: Micro-Instruction Level**

| Bug Id | Source | Target System | Type | Registered | Resolved |
| :--- | :--- | :--- | :--- | :--- | :--- |
| [L1-01](./Layer1/1-Linuxeasy.c) | https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/commit/?id=95bcade33a8a | Linux Kernel | Unsafe Publication | Feb 13, 2018 | Feb 13, 2018 |
| [L1-02](./Layer1/2-DPDK.c) | http://git.dpdk.org/dpdk/tag/?h=v20.11 | DPDK | Consistency Violation | Nov 27, 2020 | Nov 27, 2020 |
| [L1-03](./Layer1/3-seL4-dynamic.c) | https://github.com/seL4/seL4/pull/199/commits/8ba22dcdc5fa667b41db6eb80b43421a84868398 | sel4 | Unsafe Publication | Jun 22, 2020 | Jun 22, 2020 |
| [L1-04](./Layer1/4-MySQL.c) | https://bugs.mysql.com/file.php?id=28180&bug_id=94699&text=1 | MySQL | Inconsistent View | Jan 30, 2019 | Mar 04, 2019 |
| [L1-05](./Layer1/5-StackOverflowEasy.c) | http://stackoverflow.com/questions/16159203/| General Testing | Unsafe Publication | Apr 23, 2013 | May 01, 2013 |
| [L1-06](./Layer1/6-ompi.c) | https://github.com/open-mpi/ompi/issues/8274 | Open MPI | Unsafe Publication | Dec 8, 2020 | Dec 11, 2020 |
| [L1-07](./Layer1/7-OpenBLAS.c) | https://github.com/OpenMathLib/OpenBLAS/issues/2444 | OpenBLAS | Inconsistent View | Feb 25, 2020 | Apr 14, 2020 |
| [L1-08](./Layer1/8-CVE-2020-11739-Xen.c) | https://cve.mitre.org/cgi-bin/cvename.cgi?name=CVE-2020-11739 | Xen | Unsafe Publication | Apr 14, 2020 | Jul 13, 2020 |
| [L1-09](./Layer1/9-CVE-2020-25603-Xen.c) | https://cve.mitre.org/cgi-bin/cvename.cgi?name=CVE-2020-25603 | Xen | Unsafe Publication | Sep 23, 2020 | Nov 11, 2020 |
| [L1-10](./Layer1/10-ceph.c) | https://github.com/ceph/ceph/pull/38765 | Ceph | Unsafe Publication | Jan 05, 2021 | Jan 13, 2021 |
| [L1-11](./Layer1/11-MariaDBEasy.c) | https://github.com/MariaDB/server/pull/1952/commits | MariaDB | Inconsistent View | Nov 30, 2021 | Nov 30, 2021 |
| [L1-12](./Layer1/12-DragonWell8Easy.cpp) | https://bugs.openjdk.org/browse/JDK-8154750 | JDK | Unsafe Publication | Apr 20, 2016 | Jun 02, 2016 |
| [L1-13](./Layer1/13-JDK.c) | https://bugs.openjdk.org/browse/JDK-8221584| JDK | Inconsistent View | Mar 27, 2019 | Apr 08, 2019 |
| [L1-14](./Layer1/14-libfabric.c) | https://github.com/ofiwg/libfabric/pull/9370 | libfabric | Unsafe Publication | Sep 27, 2023 | Sep 27, 2023 |
| [L1-15](./Layer1/15-openmpi.c) | https://github.com/open-mpi/ompi/pull/771/files | Open MPI | Unsafe Publication | Aug 05, 2015 | Aug 05, 2015 |
| [L1-16](./Layer1/16-includeos.c) | https://github.com/includeos/IncludeOS/pull/1926/files| IncludeOS | Unsafe Publication | Oct 04, 2018 | Oct 04, 2018 |
| [L1-17](./Layer1/17-mysql.c) | https://bugs.mysql.com/bug.php?id=87293 | MySQL | Inconsistent View | Aug 03, 2017 | Oct 10, 2017 |
| [L1-18](./Layer1/18-JDK-8179954.c) | https://bugs.openjdk.org/browse/JDK-8179954 | JDK | Consistency Violation | May 09, 2017 | May 16, 2017 |
| [L1-19](./Layer1/19-ompi.cpp) | https://github.com/open-mpi/ompi/pull/5536 | Open MPI | Unsafe Publication | Aug 14, 2018 | Aug 14, 2018 |
| [L1-20](./Layer1/20-barrier.cpp) | / | General Testing | Unsafe Publication | / | / |
| [L1-21](./Layer1/21-crossbeam.c) | https://github.com/crossbeam-rs/crossbeam/issues/859 | Crossbeam | Inconsistent View | Jul 06, 2022 | Feb 14, 2024 |
| [L1-22](./Layer1/22-MariaDB.c) | https://bugs.mysql.com/bug.php?id=76135 | MariaDB | Unsafe Publication | Mar 3, 2015 | Aug 12, 2015 |
| [L1-23](./Layer1/23-mcs.c) | http://patches.dpdk.org/patch/75983/ | DPDK | Consistency Violation | Aug 26, 2020 | Feb 10, 2021 |
| [L1-24](./Layer1/24-mysql_rwlock_deadlock.c) | https://bugs.mysql.com/bug.php?id=94699 | MySQL | Inconsistent View | Mar 19, 2019 | Mar 28, 2019 |
| [L1-25](./Layer1/25-mysql_volatile_bug.c) | https://bugs.mysql.com/bug.php?id=74833 | MySQL | Unsafe Publication | Nov 13, 2014 | May 11, 2015 |
| [L1-26](./Layer1/26-mysql-99556.c) | https://bugs.mysql.com/bug.php?id=99556 | MySQL | Unsafe Publication | May 13, 2020 | May 15, 2020 |
| [L1-27](./Layer1/27-mysql-ut-realx.c) | https://bugs.mysql.com/bug.php?id=74832 | MySQL | Inconsistent View | Nov 13, 2014 | Jan 05, 2016 |
| [L1-28](./Layer1/28-pso_test.c) | / | General Testing | Unsafe Publication | / | / |
| [L1-29](./Layer1/29-sofa_pbrpc_hang.c) | https://github.com/baidu/sofa-pbrpc/issues/240 | sofa-pbrpc | Consistency Violation | May 20, 2020 | May 20, 2020 |
| [L1-30](./Layer1/30-tso_test.c) | / | General Testing | Consistency Violation | / | / |
| [L1-31](./Layer1/31-unsafe_publish.c) | https://stackoverflow.com/questions/16159203/why-does-this-java-program-terminate-despite-that-apparently-it-shouldnt-and-d | General Testing | Unsafe Publication | Apr 23, 2013 | May 1, 2013 |
| [L1-32](./Layer1/32-datarace.cpp) | / | General Testing | Unsafe Publication | / | / |

**Layer 2: Resource Lifecycle Level**

| Bug Id | Source | Target System | Type | Registered | Resolved |
| :--- | :--- | :--- | :--- | :--- | :--- |
| [L2-01](./Layer2/1-rds.c) |  | Linux Kernel | Initialization‑Use Races | Nov 20, 2024 | Dec 01, 2024 |
| [L2-02](./Layer2/2-vlan.c) | https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/commit/?id=c1102e9d49eb36c0be18cb3e16f6e46ffb717964 | Linux Kernel | Initialization‑Use Races | Apr 19, 2021 | Apr 19, 2021 |
| [L2-03](./Layer2/3-vlan-s.c) | https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/commit/?id=c1102e9d49eb36c0be18cb3e16f6e46ffb717964 | Linux Kernel | Initialization‑Use Races | Apr 19, 2021 | Apr 19, 2021 |
| [L2-04](./Layer2/4-xsk2.c) | https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/commit/?id=42fddcc7c64b723a867c7b2f5f7505e244212f13 | Linux Kernel | Initialization‑Use Races | Sep 4, 2019 | Sep 5, 2019 |
| [L2-05](./Layer2/5-fs.c) | https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/commit/?id=7ee47dcfff1835ff75a794d1075b6b5f5462cfed | Linux Kernel | Teardown‑Use Races | Oct 31, 2022 | Oct 31, 2022 |
| [L2-06](./Layer2/6-bpf.c) |  | Linux Kernel | Initialization‑Use Races |  |  |
| [L2-07](./Layer2/7-xdp2.c) |  | Linux Kernel | Initialization‑Use Races |  |  |
| [L2-08](./Layer2/8-watchqueue.c) | https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/commit/?id=2ed147f015af2b48f41c6f0b6746aa9ea85c19f3 | Linux Kernel | Initialization‑Use Races | Mar 11, 2022 | Mar 11, 2022 |
| [L2-09](./Layer2/9-tls2.c) |  | Linux Kernel | State Update Races |  |  |
| [L2-10](./Layer2/10-smc2.c) |  | Linux Kernel | Initialization‑Use Races |  |  |
| [L2-11](./Layer2/11-gsm.c) |  | Linux Kernel | State Update Races |  |  |
| [L2-12](./Layer2/12-xsk.c) | https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/commit/?id=37b076933a8e38e72ffd3c40d3eeb5949f38baf3 | Linux Kernel | Teardown‑Use Races | May 22, 2018 | May 22, 2018 |
| [L2-13](./Layer2/13-xdp.c) |  | Linux Kernel | Initialization‑Use Races |  |  |
| [L2-14](./Layer2/14-vmci.c) |  | Linux Kernel | Initialization‑Use Races |  |  |
| [L2-15](./Layer2/15-tls.c) |  | Linux Kernel | Teardown‑Use Races |  |  |
| [L2-16](./Layer2/16-nbd.c) | https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/commit/?id=c2da049f419417808466c529999170f5c3ef7d3d | Linux Kernel | Initialization‑Use Races | Nov 17, 2023 | Nov 20, 2023 |
| [L2-17](./Layer2/17-unix.c) | https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/commit/?id=ae3b564179bfd06f32d051b9e5d72ce4b2a07c37 | Linux Kernel | Initialization‑Use Races | Feb 15, 2019 | Feb 20, 2019 |
| [L2-18](./Layer2/18-unix-s.c) | https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/commit/?id=ae3b564179bfd06f32d051b9e5d72ce4b2a07c37 | Linux Kernel | Initialization‑Use Races | Feb 15, 2019 | Feb 20, 2019 |
| [L2-19](./Layer2/19-tcve-2016-8655.c) | https://nvd.nist.gov/vuln/detail/CVE-2016-8655 | Linux Kernel | State Update Races | Dec 08, 2016 | Apr 12, 2025 |
| [L2-20](./Layer2/20-tcve-2017-2636.c) | https://nvd.nist.gov/vuln/detail/CVE-2017-2636 | Linux Kernel | Teardown‑Use Races | Mar 07, 2017 | Apr 19, 2025 |
| [L2-21](./Layer2/21-tcve-2018-12232.c) | https://nvd.nist.gov/vuln/detail/CVE-2018-12232 | Linux Kernel | State Update Races | Jun 12, 2018 | Nov 20, 2024 |
| [L2-22](./Layer2/22-tcve-2019-6974.c) | https://nvd.nist.gov/vuln/detail/CVE-2019-6974 | Linux Kernel | Teardown‑Use Races | Feb 15, 2019 | Nov 20, 2024 |
| [L2-23](./Layer2/23-watchqueue.c) |  | Linux Kernel | Initialization‑Use Races |  |  |
| [L2-24](./Layer2/24-xsk.c) |  | Linux Kernel | Teardown‑Use Races |  |  |

**Layer 3: Semantic Logic Level**

| Bug Id | Source | Target System | Type | Registered | Resolved |
| :--- | :--- | :--- | :--- | :--- | :--- |
| [L3-01](./Layer3/L3-1.c) |  | Linux Kernel | State Machine Error |  |  |
| [L3-02](./Layer3/L3-2.c) | https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/commit/?id=8a0d57df8938e9fd2e99d47a85b7f37d86f91097 | Linux Kernel | State Machine Error | May 24, 2023 | May 26, 2023 |

📂 [**`CBMC`**](./Evaluation/CBMC/)
This directory contains the testing environment and evaluation data specifically adapted for the **CBMC (C Bounded Model Checker)**. Due to the nature of static formal verification, the original concurrent programs have been refactored into a specific format that CBMC can process (e.g., bounding loop limits, translating concurrency primitives, and inserting specific state assertions).

This folder mainly consists of the following three parts:
* **`Formatted Source Code`**: Concurrent bug source code restructured specifically for CBMC testing. 
* **`Automated Test Scripts`**: Scripts used to batch-execute the static verification. 
* **`Test Results`**: Contains the final output logs and evaluation metrics after the CBMC execution, documenting the bug detection results .

📂 [**`GENMC&Nidhugg`**](./Evaluation/GENMC&Nidhugg/)
This directory contains the testing environments, intermediate files, and evaluation data specifically tailored for two Stateless Model Checking (SMC) tools: **GenMC** and **Nidhugg**. 

This folder mainly consists of the following components:
* **`Formatted Source Code (for GenMC)`**: Concurrent source code adapted specifically for GenMC, utilizing the required concurrency primitives and assertion formats for its execution model.
* **`LLVM IR Files (.ll) (for Nidhugg)`**: Compiled LLVM Intermediate Representation files generated from the source code. These `.ll` files serve as the direct and necessary inputs for Nidhugg's testing process.
* **`Automated Test Scripts`**: Execution scripts designed to orchestrate the testing workflows for both tools. They handle the compilation processes, parameter configurations, and automated batch runs.
* **`Test Results`**: The final detection logs and evaluation metrics for both GenMC and Nidhugg, documenting their bug-finding capabilities.

📂 [**`IRhunter`**](./Evaluation/IRhunter/)
This directory contains the detection artifacts and utility scripts for **IRhunter**, a dynamic binary instrumentation tool used for identifying concurrency vulnerabilities.

This folder mainly consists of the following components:
* **`Vulnerability Reports`**: Detailed detection reports for 18 specific concurrency vulnerabilities identified by IRhunter during our evaluation.
* **`Automated Test Script`**: A custom-developed script designed to execute and verify IRhunter's detection logic, facilitating batch testing and reproduction of recorded bugs.
