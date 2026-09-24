Linux Kernel Monitor

A Linux-based system monitoring application developed using **C programming** for the **Operating Systems and Systems Programming (25CS2104E)** project.

 Project Overview

The **Linux Kernel Monitor** provides a centralized view of important Linux system information. It collects system information from the Linux `/proc` virtual filesystem and displays it through a simple command-line interface.

The monitor can display CPU usage, memory utilization, system uptime, load average, kernel information, and running processes.

Objectives

* Monitor CPU usage and CPU information.
* Monitor total, available, and used memory.
* Display running processes.
* Display Process ID (PID), process name, and process state.
* Display system uptime.
* Display system load average.
* Retrieve kernel information.
* Understand interaction between user-space programs and the Linux kernel.
* Implement real-time system monitoring.

 Technologies Used

* **C Programming**
* **Linux / Ubuntu**
* **Linux `/proc` Virtual Filesystem**
* **Linux System Calls**
* **POSIX APIs**
* **Linux File I/O**

 Features

### 1. CPU Monitoring

Displays CPU information and CPU utilization.

### 2. Memory Monitoring

Displays:

* Total memory
* Available memory
* Used memory
* Memory utilization percentage

### 3. Process Monitoring

Displays information about currently running processes, including:

* PID
* Process name
* Process state

### 4. System Uptime

Displays how long the Linux system has been running.

### 5. Load Average

Displays:

* 1-minute load average
* 5-minute load average
* 15-minute load average

### 6. Kernel Information

Displays Linux kernel information obtained from the `/proc` filesystem.

### 7. Live Monitoring

The application can continuously refresh system information at regular intervals.

Project Structure

```text
Kernel-Monitor/
│
├── kernel_monitor.c
├── README.md
└── screenshots/
```

 Menu

```text
============================================
          LINUX KERNEL MONITOR
============================================
1. CPU Information
2. CPU Usage
3. Memory Information
4. System Uptime
5. Load Average
6. Running Processes
7. Kernel Information
8. Display All Information
9. Live Monitoring
0. Exit
```

 How to Run

### Step 1: Clone the Repository

```bash
git clone <your-github-repository-url>
```

### Step 2: Open the Project

```bash
cd Kernel-Monitor
```

### Step 3: Compile

```bash
gcc kernel_monitor.c -o kernel_monitor
```

### Step 4: Run

```bash
./kernel_monitor
```

Linux Requirement

This project is designed for a **Linux/Ubuntu environment** because it uses the Linux `/proc` virtual filesystem and Linux system interfaces.

 Operating System Concepts

The project demonstrates:

* User Space and Kernel Space
* Process Management
* Memory Management
* CPU Utilization
* File Handling
* File Descriptors
* `/proc` Virtual Filesystem
* System Information Retrieval
* POSIX APIs
* Kernel-User Space Interaction
 Team Members

| Roll Number | Name                | Responsibility          |
| ----------- | ------------------- | ----------------------- |
| 2520030531  | D. Venya Sri        | CPU & System Monitoring |
| 2520030527  | K. Vansika Reddy    | Memory Monitoring       |
| 2520030212  | N. Rishika Chowdary | Process Monitoring      |
| 2520030060  | J. Hansika          | Integration & Testing   |

 Course

**Operating Systems and Systems Programming**

**Course Code:** 25CS2104E

**Academic Year:** 2026–27

**Term:** I

**Section:** 8

**Team:** 17

 Expected Outcome

The project provides a simple centralized interface for monitoring important Linux system information and helps demonstrate practical Operating System concepts such as process management, memory management, file I/O, system calls, and interaction between user-space applications and the operating system.
