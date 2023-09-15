#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//  you may need other standard header files


//  CITS2002 Project 1 2023
//  Student1:   23326698    Jack Blackwood   
//  Student2:   23476614    Emily Matthews


//  myscheduler (v1.0)
//  Compile with:  cc -std=c11 -Wall -Werror -o myscheduler myscheduler.c


//  THESE CONSTANTS DEFINE THE MAXIMUM SIZE OF sysconfig AND command DETAILS
//  THAT YOUR PROGRAM NEEDS TO SUPPORT.  YOU'LL REQUIRE THESE //  CONSTANTS
//  WHEN DEFINING THE MAXIMUM SIZES OF ANY REQUIRED DATA STRUCTURES.

#define MAX_DEVICES                     4
#define MAX_DEVICE_NAME                 20
#define MAX_COMMANDS                    10
#define MAX_COMMAND_NAME                20
#define MAX_SYSCALLS_PER_PROCESS        40
#define MAX_RUNNING_PROCESSES           50

//  NOTE THAT DEVICE DATA-TRANSFER-RATES ARE MEASURED IN BYTES/SECOND,
//  THAT ALL TIMES ARE MEASURED IN MICROSECONDS (usecs),
//  AND THAT THE TOTAL-PROCESS-COMPLETION-TIME WILL NOT EXCEED 2000 SECONDS
//  (SO YOU CAN SAFELY USE 'STANDARD' 32-BIT ints TO STORE TIMES).

#define DEFAULT_TIME_QUANTUM            100

#define TIME_CONTEXT_SWITCH             5
#define TIME_CORE_STATE_TRANSITIONS     10
#define TIME_ACQUIRE_BUS                20

// DEFINE THE 3 PROCESS QUEUES
//#define READY_QUEUE                     

//  ----------------------------------------------------------------------

#define CHAR_COMMENT                    '#'

//  ----------------------------------------------------------------------

#define MAX_QUEUE_SIZE                  MAX_RUNNING_PROCESSES

//  ----------------------------------------------------------------------

#define TOTAL_TIME_TAKEN 0
#define TIME_ON_CPU 0
#define CPU_UTILISATION 0

//  ----------------------------------------------------------------------

// added colours to make output easier to read
#define COLOUR_NORMAL "\x1b[0m"
#define COLOUR_RED "\x1b[31m"
#define COLOUR_YELLOW "\x1b[33m"
#define COLOUR_GREEN "\x1b[32m"
#define COLOUR_CYAN "\x1b[36m"
#define COLOUR_BLUE "\x1b[34m"
#define COLOUR_MAGENTA "\x1b[35m"
#define COLOUR_WHITE "\x1b[37m"
#define COLOUR_BLACK "\x1b[30m"

//  ----------------------------------------------------------------------

//Create a struct for the device info
typedef struct {
    char devicename[MAX_DEVICE_NAME];
    long long readspeed;
    long long writespeed;
} DeviceInfo;
//Create a struct for the device storage
typedef struct {
    DeviceInfo devices[MAX_DEVICES];
    int num_devices;
} DeviceStorage;

//Create a struct for the syscall
typedef struct {
    char syscall[MAX_COMMAND_NAME];
    int elapsed_time;
    char arg1[MAX_COMMAND_NAME];
    char arg2[MAX_COMMAND_NAME];
} Syscall;

//Create a struct for the command
typedef struct {
    char name[MAX_COMMAND_NAME];
    Syscall syscalls[MAX_SYSCALLS_PER_PROCESS];
    int num_syscalls;
} Command;

//Create a struct for the command storage
typedef struct {
    Command commands[MAX_COMMANDS];
    int num_commands;
} CommandStorage;

#define MAX_WAITING_PROCESSES 10
int waitingProcesses[MAX_WAITING_PROCESSES];
int numWaitingProcesses = 0;
//  ----------------------------------------------------------------------

/* CPU EXECUTION QUEUE */

//  Define the states
typedef enum {READY, RUNNING, BLOCKED, EXIT} State;

//Create a struct for the process
typedef struct {
    Command command;
    State state;
    int id;
    int waiting_for_process_id;
    int parent_id;
    int current_call;
    int total_time;
} Process;

//Create a struct for the queue
typedef struct {
    Process* processes[MAX_RUNNING_PROCESSES];
    int num_processes;
    int front;
    int rear;
} Queue;
int processID = 0;
Queue readyQueue, runningQueue, blockedQueue, exitQueue;
CommandStorage commandStorage;
DeviceStorage deviceStorage;

void initializeQueue(Queue* queue) {
    queue->num_processes = 0;
    queue->front = 0;
    queue->rear = -1;
}

void initProcess(Process* process) {
    process->state = READY;
    process->current_call = 0;
    process->id = -1;
    process->parent_id = -1;
    process->waiting_for_process_id = -1;

}

int isEmpty(Queue* queue) {
    return queue->num_processes == 0;
}

// Check if the queue is full
int isFull(Queue* queue) {
    return queue->num_processes == MAX_QUEUE_SIZE;
}



int enqueue(Queue* queue, const Process* process) {
    if (isFull(queue)) {
        printf("%sError: Queue is full%s\n", COLOUR_RED, COLOUR_NORMAL);
        return 0; // Return 0 to indicate failure
    } else {
        queue->rear = (queue->rear + 1) % MAX_QUEUE_SIZE;
        // Create a copy of the process and enqueue the copy
        Process* processCopy = malloc(sizeof(Process));
        if (processCopy == NULL) {
            // Handle memory allocation error
            printf("%sError: Memory allocation failed%s\n", COLOUR_RED, COLOUR_NORMAL);
            return 0;
        }
        *processCopy = *process; // Copy the process data
        queue->processes[queue->rear] = processCopy;
        queue->num_processes++;
        return 1; // Return 1 to indicate success
    }
}

int dequeue(Queue* queue, Process* process) {
    if (isEmpty(queue)) {
        printf("%sError: Queue is empty%s\n", COLOUR_RED, COLOUR_NORMAL);
        return 0; // Return 0 to indicate failure
    } else {
        // Dequeue the process
        *process = *(queue->processes[queue->front]);
        // Free the memory of the dequeued process
        free(queue->processes[queue->front]);
        queue->front = (queue->front + 1) % MAX_QUEUE_SIZE;
        queue->num_processes--;
        return 1; // Return 1 to indicate success
    }
}

//  ----------------------------------------------------------------------

/* EXECUTE SYSCALL */

int convertToInt(char value[]) {
    int newVal;
    newVal = atoi(value);
    return newVal;
}

double deviceCalculator(int speed, int amount) {
    double f_speed = (float)speed;
    double f_amount = (float)amount;
    double secondsTaken = f_amount / f_speed;
    // 1 second = 1000000 microseconds
    int usecsTaken = secondsTaken * 1000000;
    return usecsTaken;
}

// oh you already did that
int timeQuantumCheck(int time) {
    if (time == DEFAULT_TIME_QUANTUM || time > DEFAULT_TIME_QUANTUM) {
        return 0; // if time exceeds 100 which is default time quantum
    }
    return 1;
}

int handleSleepSyscall(Process* process, Syscall syscall) {
    printf("%sExecuting syscall %s%s\n", COLOUR_CYAN, syscall.syscall, COLOUR_NORMAL);
    int sleeptime = convertToInt(syscall.arg1);
    printf("%ssleeping for %d usecs%s\n", COLOUR_CYAN, sleeptime, COLOUR_NORMAL);
    process->total_time += sleeptime;
    
    // Implement sleep logic here

    return 0; // Continue the loop
}

int handleReadSyscall(Process* process, Syscall syscall, DeviceStorage* deviceStorage) {
    printf("%sExecuting syscall %s%s\n", COLOUR_CYAN, syscall.syscall, COLOUR_NORMAL);
    char devicename[MAX_DEVICE_NAME];
    strcpy(devicename, syscall.arg1);
    long long readspeed;
    double readtime;
    int bytes = convertToInt(syscall.arg2);
    int deviceInStorage = 0;

    for (int j = 0; j < deviceStorage->num_devices; j++) {
        if (strcmp(deviceStorage->devices[j].devicename, syscall.arg1) == 0) {
            deviceInStorage = 1;
            readspeed = deviceStorage->devices[j].readspeed;
            readtime = deviceCalculator(readspeed, bytes);
            break;
        }
    }

    if (deviceInStorage) {
        printf("device %s will read %d bytes in %f usecs.\n", devicename, bytes, readtime);
        syscall.elapsed_time += readtime;
    } else {
        printf("%sDevice %s not found%s\n", COLOUR_RED, devicename, COLOUR_NORMAL);
    }

    TOTAL_TIME_TAKEN += readtime;

    return 0; // Continue the loop
}

int handleWriteSyscall(Process* process, Syscall syscall, DeviceStorage* deviceStorage) {
    printf("%sExecuting syscall %s%s\n", COLOUR_CYAN, syscall.syscall, COLOUR_NORMAL);
    char devicename[MAX_DEVICE_NAME];
    strcpy(devicename, syscall.arg1);
    long long writespeed;
    double writetime;
    int bytes = convertToInt(syscall.arg2);
    int deviceInStorage = 0;

    for (int j = 0; j < deviceStorage->num_devices; j++) {
        if (strcmp(deviceStorage->devices[j].devicename, syscall.arg1) == 0) {
            deviceInStorage = 1;
            writespeed = deviceStorage->devices[j].writespeed;
            writetime = deviceCalculator(writespeed, bytes);
            break;
        }
    }

    if (deviceInStorage) {
        printf("device %s will write %d bytes in %f usecs.\n", devicename, bytes, writetime);
        syscall.elapsed_time += writetime;
    } else {
        printf("%sDevice %s not found%s\n", COLOUR_RED, devicename, COLOUR_NORMAL);
    }

    TOTAL_TIME_TAKEN += writetime;

    return 0; // Continue the loop
}

int handleSpawnSyscall(Process* process, Syscall syscall, CommandStorage* commandStorage) {
    printf("%sExecuting syscall %s%s\n", COLOUR_CYAN, syscall.syscall, COLOUR_NORMAL);
    char commandName[MAX_COMMAND_NAME];
    strcpy(commandName, syscall.arg1);
    int commandFound = 0;

    for (int j = 0; j < commandStorage->num_commands; j++) {
        if (strcmp(commandStorage->commands[j].name, commandName) == 0) {
            Process spawnedProcess;
            initProcess(&spawnedProcess);
            spawnedProcess.command = commandStorage->commands[j];
            spawnedProcess.state = READY;
            spawnedProcess.id = process->id + 1;
            processID++;
            spawnedProcess.parent_id = process->id;
            enqueue(&readyQueue, &spawnedProcess);
            printf("%sMoving Spawned Process ID %d with name %s to ready queue%s\n", COLOUR_YELLOW, spawnedProcess.id, spawnedProcess.command.name, COLOUR_NORMAL);

            process->state = READY;
            enqueue(&readyQueue, process);
            dequeue(&runningQueue, process);
            printf("%sMoving Process ID %d with name %s to ready queue (spawned Process) %s\n", COLOUR_YELLOW, process->id, process->command.name, COLOUR_NORMAL);
            commandFound = 1;
            break;
        }
    }

    if (!commandFound) {
        printf("%sCommand %s not found%s\n", COLOUR_RED, commandName, COLOUR_NORMAL);
        return 0;
    }

    return 1; // Terminate the loop
}

int handleWaitSyscall(Process* process, Syscall syscall) {
    printf("%sExecuting syscall %s%s\n", COLOUR_CYAN, syscall.syscall, COLOUR_NORMAL);
    int processToWaitFor = -1;

    for (int j = readyQueue.front; j != (readyQueue.rear + 1) % MAX_QUEUE_SIZE; j = (j + 1) % MAX_QUEUE_SIZE) {
        if (readyQueue.processes[j]->parent_id == process->id) {
            processToWaitFor = readyQueue.processes[j]->id;
            break;
        }
    }

    if (processToWaitFor == -1) {
        for (int j = exitQueue.front; j != (exitQueue.rear + 1) % MAX_QUEUE_SIZE; j = (j + 1) % MAX_QUEUE_SIZE) {
            if (exitQueue.processes[j]->parent_id == process->id) {
                processToWaitFor = -1;
                break;
            }
        }
    }

    if (processToWaitFor != -1) {
        
        printf("Process to wait for: %d\n", processToWaitFor);
        process->state = BLOCKED;
        process->waiting_for_process_id = processToWaitFor;
        printf("%sMoving Process ID %d with name %s to blocked queue%s\n", COLOUR_YELLOW, process->id, process->command.name, COLOUR_NORMAL);
        enqueue(&blockedQueue, process);
        dequeue(&runningQueue, process);
        waitingProcesses[numWaitingProcesses++] = process->id;
        return 1; // Terminate the loop
    } else {
        printf("%sNo child process to wait for or child has already finished.%s\n", COLOUR_BLACK, COLOUR_NORMAL);
        process->state = READY;
        enqueue(&readyQueue, process);
        dequeue(&runningQueue, process);
        printf("%sMoving Process ID %d with name %s to ready queue%s\n", COLOUR_YELLOW, process->id, process->command.name, COLOUR_NORMAL);
        return 1; // Terminate the loop
    }
}

int handleExitSyscall(Process* process, Syscall syscall) {
    printf("%sExecuting syscall %s%s\n", COLOUR_CYAN, syscall.syscall, COLOUR_NORMAL);
    process->state = EXIT;
    enqueue(&exitQueue, process);
    dequeue(&runningQueue, process);
    printf("%sMoving Process ID %d with name %s to exit queue%s\n", COLOUR_MAGENTA, process->id, process->command.name, COLOUR_NORMAL);

    // Unblock any processes waiting for this process
    for (int l = 0; l < numWaitingProcesses; l++) {
        int waitingProcessID = waitingProcesses[l];
        if (waitingProcessID == process->parent_id) {
            for (int o = blockedQueue.front; o != (blockedQueue.rear + 1) % MAX_QUEUE_SIZE; o = (o + 1) % MAX_QUEUE_SIZE) {
                if (blockedQueue.processes[o]->id == waitingProcessID) {
                    blockedQueue.processes[o]->state = READY;
                    enqueue(&readyQueue, blockedQueue.processes[o]);
                    printf("%sMoving Process ID %d with name %s to ready queue (unblocked)%s\n", COLOUR_YELLOW, waitingProcessID, blockedQueue.processes[o]->command.name, COLOUR_NORMAL);
                    for (int p = l; p < numWaitingProcesses - 1; p++) {
                        waitingProcesses[p] = waitingProcesses[p + 1];
                    }
                    numWaitingProcesses--;
                    l--;
                    break;
                }
            }
        }
    }
    return 1; // Continue the loop
}

void executeSyscall(Process* process, DeviceStorage* deviceStorage, CommandStorage* commandStorage) {
    int shouldTerminate = 0;

    for (int i = process->current_call; i < process->command.num_syscalls && !shouldTerminate; i++) {
        process->current_call = i;
        Syscall syscall = process->command.syscalls[i];

        if (strcmp(syscall.syscall, "sleep") == 0) {
            shouldTerminate = handleSleepSyscall(process, syscall);
        } else if (strcmp(syscall.syscall, "read") == 0) {
            shouldTerminate = handleReadSyscall(process, syscall, deviceStorage);
        } else if (strcmp(syscall.syscall, "write") == 0) {
            shouldTerminate = handleWriteSyscall(process, syscall, deviceStorage);
        } else if (strcmp(syscall.syscall, "spawn") == 0) {
            process->current_call = i + 1;
            shouldTerminate = handleSpawnSyscall(process, syscall, commandStorage);
        } else if (strcmp(syscall.syscall, "wait") == 0) {
            process->current_call = i + 1;
            shouldTerminate = handleWaitSyscall(process, syscall);
            break;
        } else if (strcmp(syscall.syscall, "exit") == 0) {
            shouldTerminate = handleExitSyscall(process, syscall);
        }
        // if (process->total_time > DEFAULT_TIME_QUANTUM) {
        //     process->state = READY;
        //     enqueue(&readyQueue, process);
        //     dequeue(&runningQueue, process);
        //     printf("%sMoving Process ID %d with name %s to ready queue%s\n", COLOUR_YELLOW, process->id, process->command.name, COLOUR_NORMAL);
        // }

        if (shouldTerminate) {
            break;
        }
    }
}

//  ----------------------------------------------------------------------

/* EXECUTE COMMANDS */

void execute_commands(CommandStorage* commandStorage, DeviceStorage* deviceStorage) {
    // Create a queue for each state

    // Initialize the queues
    initializeQueue(&readyQueue);
    initializeQueue(&runningQueue);
    initializeQueue(&blockedQueue);
    initializeQueue(&exitQueue);

    //int globalTime = 0;

    // Create an initial process for each command and add it to the ready queue
    for (int i = 0; i < commandStorage->num_commands; i++) {
        Process process;
        initProcess(&process);
        process.command = commandStorage->commands[i];
        process.state = READY;
        process.id = processID++;
        process.parent_id = -1;
        enqueue(&readyQueue, &process);
        printf("%sAdding Process ID %d with name %s to ready queue%s\n", COLOUR_YELLOW, i, process.command.name, COLOUR_NORMAL);
    

    // While there are still processes in the running queue or ready queue, execute them
    while (!isEmpty(&runningQueue) || !isEmpty(&readyQueue)) {
        if (!isEmpty(&readyQueue)) {
            dequeue(&readyQueue, &process);
            process.state = RUNNING;
            enqueue(&runningQueue, &process);
            printf("%sMoving Process ID %d with name %s to running queue%s\n", COLOUR_GREEN, process.id, process.command.name, COLOUR_NORMAL);
        }

        // Execute processes in the running queue
        if (!isEmpty(&runningQueue)) {
            executeSyscall(&process, deviceStorage, commandStorage);
        }
    }
}
}

void initializeCommandStorage(CommandStorage *storage) {
    storage->num_commands = 0;
}

void initializeDeviceStorage(DeviceStorage *storage) {
    storage->num_devices = 0;
}

void addCommand(CommandStorage *storage, Command command) {
    if (storage->num_commands < MAX_COMMANDS) {
        storage->commands[storage->num_commands] = command;
        storage->num_commands++;
    } else {
        printf("%sError: Maximum number of commands reached.%s\n", COLOUR_RED, COLOUR_NORMAL);
    }
}

void addDevice(DeviceStorage *storage, DeviceInfo device) {
    if (storage->num_devices < MAX_DEVICES) {
        storage->devices[storage->num_devices] = device;
        storage->num_devices++;
    } else {
        printf("%sError: Maximum number of devices reached.%s\n", COLOUR_RED, COLOUR_NORMAL);
    }
}

//  ----------------------------------------------------------------------

/* READ SYSCONFIG AND COMMANDS */

void read_sysconfig(char argv0[], char filename[], DeviceStorage *deviceStorage)
{

    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("unable to open '%s'\n", filename);
        exit(EXIT_FAILURE);
    }
    char buffer[9999];

    while (fgets(buffer, sizeof buffer, fp) != NULL) {
        if (buffer[0] == CHAR_COMMENT || buffer[0] == '\n') {
            // Ignore comments and empty lines
            continue;
        }
        char token[100];

        if (sscanf(buffer, "%s", token) == 1) {
            if (strcmp(token, "device") == 0) {
                char devicename[MAX_DEVICE_NAME];
                long long readspeed;
                long long writespeed;
                // out of buffer find first string and ignore, then find devicename string store it, then find readspeed and writespeed as long long and store
                if(sscanf(buffer, "%*s %s %lldBps %lldBps", devicename, &readspeed, &writespeed) == 3){
                    DeviceInfo device;
                    strcpy(device.devicename, devicename);
                    device.readspeed = readspeed;
                    device.writespeed = writespeed;
                    addDevice(deviceStorage, device);
                }
                //No idea what this is for
            } else if (strcmp(token, "timequantum") == 0) {
                int timequantum;
                if (sscanf(buffer, "%*s %d", &timequantum) == 1) {
                    printf("Time Quantum: %dusec\n", timequantum);
                }
            }
        }

    }

    fclose(fp);
}

void read_commands(char argv0[], char filename[], CommandStorage *commandStorage)
{
    // open the commands file
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("unable to open '%s'\n", filename);
        exit(EXIT_FAILURE);
    }

    char buffer[9999];
    Command current_command;
    current_command.num_syscalls = 0;
    int newCommand = 0;

    while (fgets(buffer, sizeof buffer, fp) != NULL) {
        // ignore comments and new line
        if (buffer[0] == CHAR_COMMENT || buffer[0] == '\n') continue;
        
        if (buffer[0] != '\t'){
            //Only add the previous current command once we come across a new command
            if (newCommand) {
                addCommand(commandStorage, current_command);
                current_command.num_syscalls = 0;
                newCommand = 0;
            }
            // Parse the command name
            sscanf(buffer, "%s", current_command.name);
            newCommand = 1;
        } else {
            Syscall syscall;
            int parsed_fields = sscanf(buffer, " %dusecs %s %s %s", &syscall.elapsed_time, syscall.syscall, syscall.arg1, syscall.arg2);

            if (parsed_fields >= 2) {
                // At least the elapsed time and syscall name were successfully parsed.
                // Check if the arguments are empty and set them to empty strings if necessary.
                if (parsed_fields < 3) {
                    // Argument 1 is empty
                    strcpy(syscall.arg1, "");
                }
                if (parsed_fields < 4) {
                    // Argument 2 is empty
                    strcpy(syscall.arg2, "");
                }

                // Add the syscall to the current command.
                current_command.syscalls[current_command.num_syscalls++] = syscall;
            } else {
                printf("%sError parsing line: %s%s\n", COLOUR_RED, buffer, COLOUR_NORMAL);
            }
            }
    }
    // Add the last command
    if (newCommand) {
        addCommand(commandStorage, current_command);
    }

    // finished with the file
    fclose(fp);
}

//  ----------------------------------------------------------------------

/* FINAL CALCULATIONS */

void calculateTotalTimeTaken() {}

void calculateCPUUtilisation() {}

//  ----------------------------------------------------------------------

void printCommandStorage(CommandStorage *storage) {
    for (int i = 0; i < storage->num_commands; i++) {
        Command command = storage->commands[i];
        printf("Command %d: %s\n", i, command.name);
        for (int j = 0; j < command.num_syscalls; j++) {
            Syscall syscall = command.syscalls[j];
            printf("    Syscall %d: %dusecs %s %s %s\n", j, syscall.elapsed_time, syscall.syscall, syscall.arg1, syscall.arg2);
        }
    }
}

void printDeviceStorage(DeviceStorage *storage) {
    for (int i = 0; i < storage->num_devices; i++) {
        DeviceInfo device = storage->devices[i];
        printf("Device %d: %s, %lldBps, %lldBps\n", i, device.devicename, device.readspeed, device.writespeed);
    }
}

//  ----------------------------------------------------------------------

int main(int argc, char *argv[])
{
    CommandStorage commandStorage;
    DeviceStorage deviceStorage;
    initializeCommandStorage(&commandStorage);
    initializeDeviceStorage(&deviceStorage);
//  ENSURE THAT WE HAVE THE CORRECT NUMBER OF COMMAND-LINE ARGUMENTS
    if(argc != 3) {
        printf("Usage: %s sysconfig-file command-file\n", argv[0]);
        exit(EXIT_FAILURE);
    }

//  READ THE SYSTEM CONFIGURATION FILE
    read_sysconfig(argv[0], argv[1], &deviceStorage);

//  READ THE COMMAND FILE
    read_commands(argv[0], argv[2], &commandStorage);

    printCommandStorage(&commandStorage);
    printDeviceStorage(&deviceStorage);


//  EXECUTE COMMANDS, STARTING AT FIRST IN command-file, UNTIL NONE REMAIN
    execute_commands(&commandStorage, &deviceStorage);

//  PRINT THE PROGRAM'S RESULTS
    printf("measurements  %i  %i\n", TOTAL_TIME_TAKEN, CPU_UTILISATION);

    exit(EXIT_SUCCESS);
}

//  vim: ts=8 sw=4