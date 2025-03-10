# Multi-Process File Synchronization and Buffered I/O in C

This repository contains a set of C exercises designed to demonstrate various file synchronization techniques and buffered I/O implementations. Each exercise addresses specific synchronization methods and file management operations.

---

## 📋 Exercises Overview

### 🛠️ **Part 1: Naive Synchronization**
- **File**: `part1.c`
- Forks two child processes. Parent and child processes write to a shared file using naive synchronization (e.g., `sleep`).

**Usage**:
```bash
./part1 "Parent message" "Child1 message" "Child2 message" 3
```

---

### 🔒 **Part 2: File Lock Synchronization**
- **File**: `part2.c`
- Implements synchronization using a lock file (`lockfile.lock`) ensuring mutual exclusion.

**Usage:**
```bash
./part2 "Message 1" "Message 2" "Message 3" 5 > output2.txt
```

### 📂 **Part 4: Directory Copying Utility**
- **File**: `part4.c`
- Recursively copies directories and files, similar to Python’s `shutil.copytree`.
- Supports symbolic links (`-l`) and permissions (`-p`).

**Usage:**
```bash
./part4 -l -p <source_directory> <destination_directory>
```

### 📂 **Buffered I/O with Pre-append Feature**
- **Files**: `buffered_open.c`, `buffered_open.h`
- Provides a custom buffered I/O system supporting a special `O_PREAPPEND` flag to allow prepending data to a file.

**Example Usage:**

```c
#include "buffered_open.h"

buffered_file_t *bf = buffered_open("example.txt", O_WRONLY | O_CREAT | O_PREAPPEND, 0644);
buffered_write(bf, "Hello, World!", strlen("Hello, World!"));
buffered_close(bf);
```

---

## 🚀 Getting Started

### **Clone the Repository**

```bash
git clone <your-repo-url>
```

### **Prerequisites**

- **GCC Compiler**
- **Linux/Unix OS** (for system calls)

### **Compile Examples**

Use the following commands to compile:
```bash
gcc -o part1 part1.c
gcc -o part2 part2.c
gcc -o part4 part4.c
gcc -o buffered_open buffered_open.c -lm
gcc -o copytree copytree.c -lm
```

---

## 👥 Contributors
- **Lior Muallem**
- **Michael Landesman Nir**

---

Feel free to contact the contributors or open an issue if you have any questions or suggestions!

