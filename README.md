# GRS v1.0 - System Resource Manager (Technical Demo)
### Deterministic and Resilient Linked Memory Architecture in Pure C++

This component is a low-level, data-type agnostic dynamic memory manager. It is designed to offer mathematical predictability, direct data access of $O(1)$ complexity without sequential traversals, absolute control over fragmentation, and a strict zero-tolerance crash-proof policy.

---

// --- Quick Integration & API Preview (from main.cpp) ---
#include "../include/GRSInterface.h"
#pragma comment(lib, "../lib/GRSWindows64.lib")
#include "../include/GRSDependencies.h"
#pragma comment(lib, "../lib/GRSDependencies.lib")

// 1. Create Instance & Inject Dependencies
GRS_INTERFACE GRSInterface;
GRSInterface.CrateInstance();

GRSInterface.DependencyInjection(
    GRS_DEPENDENCIES::AllocateMemory(WINDOWS_ALLOCATOR),
    GRS_DEPENDENCIES::SetOutputType(WINDOWS_OUTPUT)
);

// 2. Active Logs & Initial Pool Configuration
GRSInterface.EnableLogs(true, true, "Log_StressTest_With_Limit.txt");
GRSInterface.InitialConfiguration(10, 1.5, "kB", "StressTest");

// 3. Package Custom Component into Memory
TRANSFORM_COMPONENT TransforComponent = { };
GRSInterface.Package(333, "TransforComponent", {TransforComponent});

// 4. Retrieve Pointer, Modify Data & Print it
TRANSFORM_COMPONENT* transformDir = reinterpret_cast<TRANSFORM_COMPONENT*>(GRSInterface.GetValue(333));
reinterpret_cast<TRANSFORM_COMPONENT*>(transformDir)->SetRot(30.0f, 60.0f, 90.0f);
GRSInterface.FormattedOutput("\n%1f\n", reinterpret_cast<TRANSFORM_COMPONENT*>(transformDir)->GetRotY( ));

// 5. Defragment or Migrate or Both during IDLE Cycles
GRSInterface.Defragment();
GRSInterface.Migrate( );

// 6. Cleanup Memory
GRSInterface.DeleteMemory();
```[cite: 4]

---

## Initial Configuration and Optimal Lifecycle

When instantiating the manager, the developer defines the base parameters according to their application requirements:

1. **Identifier:** A unique name for the manager instance.
2. **Initial Indexed Capacity:** Base quantity of elements that the index table will register.
3. **Initial Pool Dimension:** Size of the first memory (supports from 1 KB up to $2^{32}-8$ bytes).

>**Note:** If the initial configuration correctly aligns with the actual needs of the software, the manager will operate ultra-efficiently in a single fixed pool throughout its entire lifecycle, without needing to expand.

Upon closing the program, the manager provides detailed shutdown metrics (**Profiler**), showing the number of blocks, average bytes per block, and the percentage of space used in each memory pool. This allows the developer to analyze telemetry to recalibrate the initial configuration and optimize performance.

---

## Searches and Index Table

### Direct Addressing $O(1)$ and Nomadic Table

* **Zero Sequential Search (No Memory Traversal):** The manager does not perform linear sweeps or iterations through memory pages or blocks to locate or retrieve information. Data requests are resolved directly and deterministically in constant time ($O(1)$).
* **Nomadic and Rescalable Index Table:** Information queries are delegated to a decoupled index table, equipped with a nomadic structure (capable of dynamically relocating or migrating in RAM without invalidating user references) and rescalable structure (capable of adapting geometrically based on demand density).

---

## Architecture and Safety Mechanisms (Fault Tolerance)

The system integrates three independent layers of dynamic locks and safety interlocks to ensure that the main application never suffers a crash due to memory issues:

### 1. Integrated Triple Locking System

* **System RAM Lock:** If the Operating System denies the creation of new expansion pools, the manager locks safely. It halts new allocations in new memories, but continues to operate with existing resources, allowing space optimization and data insertion into free gaps within already active pools.
* **Table Expansion Lock:** If the index table reaches its critical threshold and cannot expand geometrically due to lack of physical memory, the system freezes the entry of new blocks and maintains program stability with the hosted data, resuming normal operation if space is freed.
* **Block Size Lock:** Immediate lock upon any attempt to allocate an individual block whose size exceeds the maximum configured capacity for the initial pool.

### 2. Fragmentation Control and Damped Movement

* **Linear Allocation and Coalescence:** Data is stored contiguously. Both block deletion and damped defragmentation execute coalescence algorithms to detect and unify gaps.
* **Cost-Benefit Strategy:** Defragmentation and migration processes evaluate performance impact before moving structures and expose public methods ideal for invocation during CPU idle (IDLE) cycles.
* **Grace Period:** When a memory (real or virtual) is completely emptied, the system does not release it instantaneously. It is granted 3 grace periods as a deferred validation control mechanism.
    * **Virtual Segmentation Grace Period:** If the grace period for a virtual memory expires, the underlying `WINDOWS_ALLOCATOR` executes a marking strategy instead of returning the page to the kernel. Virtual segmentations remain in an active cache state, preventing system-level access violations and guaranteeing a recycling speed of **O(1)** when stress levels rise again.
* **Cascading Collection and Cleanup:** The manager inspects previous memories before expanding, eliminates obsolete/empty memories if the load can be absorbed by prior ones, and safely destroys all pools in reverse cascade (from last to first) upon instance termination.

---

## Technical Specifications and Compatibility

* **Absolute Independence:** Codebase 100% free of `std` and external libraries. The core of the manager features an architecture suitable for Bare-Metal environments. The current demo version is configured for Windows, but the decoupled design allows migrating the source code to other operating systems or embedded systems.
* **Compatibility:** Source code-level support for 32-bit and 64-bit architectures (automatically adjusts memory alignment according to the compilation target).
* **Flexibility:** Ability to store entire structures and classes, allowing table iteration and internal function calls beyond stack limits.

### Telemetry Abstraction (Bare-Metal Portability)

The manager is 100% hardware-agnostic and is not coupled to the input/output functions of any Operating System. To activate telemetry, the system uses dependency injection via inheritance:

* **Decoupling of Memory Allocator and Telemetry (Dependency Injection)**
To guarantee architectural portability, the system decouples raw memory acquisition from its deterministic internal management and telemetry output method (Windows Console, Files, UART in embedded systems, etc.) through two abstract interface types (`GRS::INTERFAZ_MEMORIA` and `GRS::INTERFAZ_TELEMETRÍA`). This architecture allows the system to dynamically switch between different memory backends depending on the environment:
    **`ASIGNADOR_ESTANDAR` (C++ Runtime):** Uses standard `new` and `delete` operators, providing instant cross-platform compatibility for desktop and mobile devices.
    **`ASIGNADOR_WINDOWS` (Native OS Kernel):** Leverages the Windows virtual memory subsystem via `VirtualAlloc` and `VirtualFree`. To avoid kernel-level fragmentation and context switching overhead, it forces a **64 KB page-aligned** allocation pattern, dividing it into logical virtual "segments" to achieve runtime reduction and improve overall performance.
* If log flags are active but the user does not implement a specific output, the base class safely intercepts calls via default empty virtual functions, guaranteeing zero crashes (anti-crash) and allowing the formatter to be turned off to save CPU cycles.

---

## Memory Footprint & Binary Analysis

Although the compiled Windows demonstration library (`GRSWindows64.lib`) measures **197.39 KB** on disk, this figure reflects the container overhead of the MSVC COFF format (symbol tables, section alignment, x64 relocation data, and debug checksums).

An inspection of the binary using `dumpbin /summary` reveals that the **actual runtime memory footprint** for the core logic on x64 is only **~31 KB**:

| Section | Size (Hex) | Size (Bytes) | Memory Type | Description |
| :--- | :--- | :--- | :--- | :--- |
| **`.text$mn`** | `0x67F1` | **26,609 B (~26.0 KB)** | Code (Flash / ROM) | Machine instructions for memory allocation and index management. |
| **`.rdata`** | `0x139E` | **5,022 B (~4.9 KB)** | Read-Only (Flash / ROM) | Constant lookup tables and static metadata. |
| **`.pdata` + `.xdata`** | `0x0E14` | 3,604 B (~3.5 KB) | Windows Overhead | x64 stack unwinding and exception handling (not present in bare-metal). |
| **`.chks64` + `.debug$S`** | `0x18CC` | 6,348 B (~6.2 KB) | Build Metadata | Linker checksums and debug symbol tables (stripped in release binaries). |
| **`.data` + `.bss`** | `0x010C` | 268 B | State (RAM) | Global and static system state variables. |

### Key Architecture Takeaways

* **Effective Binary Footprint:** The core execution engine (`.text` + `.rdata`) requires only **~31 KB** in a 64-bit desktop environment.
* **Bare-Metal Projections:** When compiled for 32-bit embedded architectures (e.g., ARM Cortex-M or RISC-V) with space optimizations (`-Os`) and without Windows x64 exception structures (`.pdata`/`.xdata`), the final binary footprint drops even further, fitting comfortably within tight SRAM/Flash hardware limits.

---

## Demo Version Restrictions

* **Format:** Compiled as an optimized static library (`.lib`) and tested in Windows environments (MSVC / Build Tools), solely for the purpose of facilitating execution and evaluation in standard development environments.
* **Reservation Limits:** Restricted to a total maximum of **16 KB (16,384 bytes)**. This artificial limit intentionally triggers the same native Anti-Crash System routines to allow evaluation and stress testing of locking mechanisms in development environments.

**Proprietary Notice:** Closed-source software with reserved intellectual property. Published exclusively as a technical portfolio to demonstrate capabilities in low-level software development, system architecture, and resource optimization. Available for employment opportunities.

---

## Contact & Support

If you wish to use GRS in a commercial project or require a custom implementation, feel free to contact me:

* **Email:** [marcos.gorostegui@hotmail.com](mailto:marcos.gorostegui@hotmail.com)
* **LinkedIn:** [linkedin.com/in/marcos-eugenio-gorostegui-929189432](https://www.linkedin.com/in/marcos-eugenio-gorostegui-929189432/)

### Support the Developer

If this technical demonstration or its architectural concepts have been useful to you, you can support my independent research and development here:

* **PayPal:** [paypal.me/utopianre](https://paypal.me/utopianre)

### Bug Reporting and Technical Feedback

Since this version is a technical demonstration oriented towards architectural evaluation, the software is distributed "as-is" and without explicit production warranties.
However, if during stress testing you detect any anomalous behavior, telemetry discrepancy, or edge case not covered by the Anti-Crash system, I would appreciate sending the report along with Profiler metrics to:

* **Issue Reporting Email:** [marcos.gorostegui@hotmail.com](mailto:marcos.gorostegui@hotmail.com)
