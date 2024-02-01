# myscheduler

`myscheduler` is a C-based project designed to simulate a scheduler for managing processes and their interactions with various system resources. The program reads system configuration and commands from files, and then executes these commands according to the specified system calls.

## Features

- Process simulation with state management (READY, RUNNING, BLOCKED, EXIT).
- Handling of system calls like `sleep`, `read`, `write`, `spawn`, `wait`, and `exit`.
- Simulation of device interaction with read/write operations.
- Calculation of CPU utilization and total process completion time.

## Getting Started

### Prerequisites

To run this project, you will need a C compiler like `gcc` and standard build tools.

### Compilation

Compile the program with the following command:

```bash
cc -std=c11 -Wall -Werror -o myscheduler myscheduler.c
```
To run the program, use the following command:
```bash
./myscheduler <sysconfig-file> <command-file>
```
Replace <sysconfig-file> and <command-file> with the paths to your system configuration file and command file, respectively.

### Usage
After compiling the program, you can run it by providing two input files:

- System Configuration File (sysconfig-file): This file should contain the configuration of the system, including device details and time quantum.

- Command File (command-file): This file should contain the commands to be executed, along with their associated system calls.

# Authors
Jack Blackwood - 23326698
Emily Matthews - 23476614
