# GRS v1.0 - API Documentation & Reference Manual

Complete API reference manual for the **GRS (Gestor de Recursos del Sistema)** dynamic memory manager library.

---

## 1. Core Lifecycle & Setup

### `void CrateInstance(void)`
Instantiates and initializes the memory manager core.

### `void DependencyInjection(void* AllocatorType, void* OutputType)`
Injects custom hardware/OS dependencies for memory allocation and output stream telemetry.
* **`AllocatorType`**: Pointer to custom allocator implementation (e.g., `STANDARD_ALLOCATOR`, `WINDOWS_ALLOCATOR`).
* **`OutputType`**: Pointer to custom output implementation (e.g., `NULL_OUTPUT`, `WINDOWS_OUTPUT`).

### `void InitialConfiguration(unsigned int tableElements, const float memorySpace, const char* memoryUnits, const char* instanceName)`
Configures the initial operational parameters for the manager.
* **`tableElements`**: Base capacity of items that the Index Table can track initially.
* **`memorySpace`**: Numeric size of the initial memory pool.
* **`memoryUnits`**: Unit string representing the memory scale (e.g., `"B"`, `"KB"`, `"MB"`, `"GB"`).
* **`instanceName`**: Unique textual identifier assigned to this manager instance.

---

## 2. Formatted Output Utility

### `void FormattedOutput(const char* text, Args... rest)`
C-style formatted printing utility supporting custom telemetry and bare-metal output sinks.

#### Supported Format Specifiers
| Specifier | Data Type / Description |
| :--- | :--- |
| `0x%h` | Hexadecimal memory address |
| `%c` | Single `char` character |
| `%s` | Null-terminated string (`const char*`) |
| `%i` | Signed integer (`int`) |
| `%u` | Unsigned integer (`unsigned int`) |
| `%Nf` | Single-precision floating point (`float`), where `N` is the number of decimal digits |
| `%Nd` | Double-precision floating point (`double`), where `N` is the number of decimal digits |

---

## 3. Memory Allocation & Packaging

### Buffer-Type Blocks (Identifier + Data)

#### `template<typename T, unsigned int M> bool Reserve(unsigned int identifier)`
Pre-allocates contiguous space for an array of `M` elements of type `T` associated with `identifier`. Bypasses stack-allocation limits.
* **Returns**: `true` if reservation succeeded; `false` if blocked by safety limits.

#### `template<typename T, unsigned int M> bool Package(unsigned int identifier, const T(&value)[M])`
Directly writes and packages an array `value` of `M` elements into memory associated with `identifier`.
 **Returns**: `true` if reservation succeeded; `false` if blocked by safety limits.

---

### Word-Type Blocks (Identifier + Key + Data)

#### `template<unsigned int N, typename T, unsigned int M> bool Reserve(unsigned int identifier, const char(&key)[N])`
Pre-allocates contiguous space for an array of `M` elements of type `T` bound to a key string `key` of length `N` associated with `identifier`. Bypasses stack-allocation limits.
 **Returns**: `true` if reservation succeeded; `false` if blocked by safety limits.

#### `template<unsigned int N, typename T, unsigned int M> bool Package(unsigned int identifier, const char(&key)[N], const T(&value)[M])`
Directly writes and packages an array `value` of `M` elements into memory alongside its string key `key` associated with `identifier`.
 **Returns**: `true` if reservation succeeded; `false` if blocked by safety limits.

---

## 4. Deletion & Memory Cleanup

### `void DeleteBlockByIndex(unsigned int index)`
Frees and unregisters a data block using its direct position index within the Index Table.

### `void DeleteBlockByIdentifier(unsigned int identifier)`
Frees and unregisters a data block using its assigned numeric identifier.

### `void DeleteMemory(void)`
Completely destroys all active memory pools and resets the manager instance.

---

## 5. Inspection, Queries & Table Inspection

### `const char* GetInstanceName(void)`
Retrieves the string identifier assigned to the current manager instance.

### `void PrintTable(void)`
Prints the structural layout of the Index Table, displaying addresses of all registered blocks and available free gaps.

### `unsigned int GetTableVersion(void)`
Returns the current structural version counter of the Index Table (incremented upon reallocations/rescaling).

### `unsigned int GetTableRecordCount(void)`
Returns the maximum record capacity currently supported by the Index Table.

### `unsigned char* GetDataFromTable(const unsigned int index)`
Fetches the raw memory address of a block using its Index Table index.
> **⚠️ NOTE:** The returned address is intended strictly for reading, writing, or overwriting data. The caller is responsible for not altering pointer arithmetic or corrupting adjacent blocks.

### `unsigned int GetMemoryLevel(const unsigned int index)`
Returns the specific memory pool level/tier where the block at `index` resides.

---

## 6. Maintenance & Optimization

### `void Defragment(void)`
Executes coalescing and defragmentation routines across active memory pools.
* *Usage:* Designed to be invoked manually during CPU idle (`IDLE`) cycles.

### `void Migrate(void)`
Evaluates cost-benefit metrics and consolidates sparse blocks across memory pools to optimize space usage.

---

## 7. Telemetry & Logging

### `void EnableLogs(bool useConsole, bool useFile, const char* fileName)`
Enables logging output streams.
* **`useConsole`**: Routes log messages to console output.
* **`useFile`**: Routes log messages to a disk file.
* **`fileName`**: Output log file path (ignored if `useFile` is `false`).

### `void DisableLogs(void)`
Disables all logging streams and suppresses telemetry formatting.

---

## 8. Block Manipulation API

### Word-Type Specific Functions

#### `unsigned int GetKeyLength(unsigned int identifier)`
Returns the character length of the string key associated with `identifier`.

#### `char* GetKey(unsigned int identifier)`
Retrieves a pointer to the key string bound to `identifier`.

#### `bool SetKey(unsigned int identifier, const char* newKey)`
Replaces the block's current key with `newKey`.
* **Constraint**: `newKey` must match the exact string length of the original key.

#### `void PrintKey(unsigned int identifier)`
Outputs the string key associated with `identifier`.

---

### General Block Functions (Buffer & Word Types)

#### `bool SetIdentifier(unsigned int currentIdentifier, unsigned int newIdentifier)`
Updates a block's numeric identifier from `currentIdentifier` to `newIdentifier`.

#### `unsigned int GetRecordValueCount(unsigned int identifier)`
Returns the total count of data elements stored within the block corresponding to `identifier`.

#### `void* GetValue(unsigned int identifier)`
Retrieves a direct pointer to the payload data stored under `identifier`.
> **⚠️ NOTE:** The returned address is intended strictly for reading, writing, or overwriting data. The caller is responsible for not altering pointer arithmetic or corrupting adjacent blocks.

#### `void PrintValue(unsigned int identifier)`
Prints the contents of the block matching `identifier` (intended for non-massive data sets).

#### `void PrintMemory(void)`
Outputs the entire contents of all data blocks across active memory pools.
> **⚠️ CAUTION:** Exercise care when invoking on large memory pools due to potential output volume.
