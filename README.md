# 🖥️ Operating System Kernel Simulator

This project is a **Complex Engineering Activity (CEA)** developed for the **Operating Systems Lab (CE332L)** course at **Air University**.

It is a CLI-based simulator that models the core functionalities of an Operating System Kernel, including Process Management, Memory Management, and CPU Scheduling.

## 🚀 Features

### 1. Process Management
- Implements a **Process Control Block (PCB)** structure.
- Tracks process states: `NEW`, `READY`, `RUNNING`, `TERMINATED`.
- Manages attributes like PID, Arrival Time, Burst Time, and Priority.

### 2. Memory Management
- Simulates Main Memory (RAM) as a fixed-size array (1024 MB).
- Uses the **First-Fit Allocation Algorithm** to efficiently load processes.
- Ensures processes are only moved to the Ready Queue if sufficient contiguous memory is available.

### 3. CPU Scheduling Algorithms
The simulator supports four distinct scheduling algorithms, selectable at runtime:
- **FCFS** (First Come First Serve)
- **SJF** (Shortest Job First - Non-Preemptive)
- **Priority Scheduling** (Non-Preemptive)
- **Round Robin** (Preemptive with Time Quantum)

## 🛠️ Tech Stack
- **Language:** C (Standard C99/C11)
- **Compiler:** GCC (MinGW-w64)
- **Architecture:** Modular Design (`main.c`, `modules.c`, `os_sim.h`)

## ⚙️ How to Run
Since the project is modular, you need to compile the source files together.

1. **Clone the Repository:**
   ```bash
   git clone [https://github.com/YourUsername/OS-Kernel-Simulator-CEA.git](https://github.com/YourUsername/OS-Kernel-Simulator-CEA.git)
2. **Compile:**

gcc main.c modules.c -o os_sim

Run:

On Windows:

.\os_sim

On Linux/Mac:

./os_sim
