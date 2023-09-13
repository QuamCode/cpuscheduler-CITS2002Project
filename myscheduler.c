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

//  ----------------------------------------------------------------------

/* CPU EXECUTION QUEUE */

//  Define the states
typedef enum {READY, RUNNING, BLOCKED, EXIT} State;

//Create a struct for the process
typedef struct {
    Command command;
    State state;
    int id;
    int waiting_process_id;
    int parent_id;
    int current_call;
    int total_time;
} Process;

//Create a struct for the queue
typedef struct {
    Process processes[MAX_RUNNING_PROCESSES];
    int num_processes;
    int front;
    int rear;
    int waiting_process_id;
} Queue;

Queue readyQueue, runningQueue, blockedQueue, exitQueue;
CommandStorage commandStorage;
DeviceStorage deviceStorage;

void initializeQueue(Queue *queue) {
    queue->num_processes = 0;
    queue->front = 0;
    queue->rear = -1;
}

void initProcess(Process *process) {
    process->state = READY;
    process->id = -1;
    process->parent_id = -1;
    process->waiting_process_id = -1;

}

int isEmpty(Queue *queue) {
    return queue->num_processes == 0;
}

// Check if the queue is full
int isFull(Queue *queue) {
    return queue->num_processes == MAX_QUEUE_SIZE;
}


void enqueue(Queue *queue, Process process) {
    if(isFull(queue)) {
        printf("%sError: Queue is full%s\n", COLOUR_RED, COLOUR_NORMAL);
    } else {
        queue->rear = (queue->rear + 1) % MAX_QUEUE_SIZE;
        queue->processes[queue->rear] = process;
        queue->num_processes++;
    }

}

Process dequeue(Queue *queue) {
    if(isEmpty(queue)) {
        printf("%sError: Queue is empty%s\n", COLOUR_RED, COLOUR_NORMAL);
        Process process;
        initProcess(&process);
        return process;
    } else {
        Process process = queue->processes[queue->front];
        queue->front = (queue->front + 1) % MAX_QUEUE_SIZE;
        queue->num_processes--;
        return process;
    }
}

//  ----------------------------------------------------------------------

/* EXECUTE SYSCALL AND COMMANDS */

int convertToInt(char value[]) {
    int newVal;
    newVal = atoi(value);
    return newVal;
}

double deviceCalculator(int speed, int amount) {
    double f_speed = (float)speed;
    double f_amount = (float)amount;
    double secondsTaken = f_amount/f_speed;
    // 1 second = 1000000 microseconds
    int usecsTaken = secondsTaken * 1000000;
    return usecsTaken;
}

void executeSyscall(Process *process, DeviceStorage *deviceStorage, CommandStorage *commandStorage) {
    //printf("bps conversions: %d %d\n", readspeed, writespeed);
    for (int i = process->current_call; i < process->command.num_syscalls; i++) {
        process->current_call = i;
        Syscall syscall = process->command.syscalls[i];
        //Check if the syscall is a sleep, read, write, spawn or exit
        if (strcmp(syscall.syscall, "sleep") == 0) {
            printf("Executing syscall %s\n", syscall.syscall);
            //syscall.arg1 is the sleep time
            //Sleep for the elapsed time
            int sleeptime = convertToInt(syscall.arg1);
            printf("sleeping for %d usecs\n", sleeptime);
            process->total_time += sleeptime;
        } else if (strcmp(syscall.syscall, "read") == 0) {
            printf("Executing syscall %s\n", syscall.syscall);
            //syscall.arg1 is the device name, syscall.arg2 is the number of bytes to read
            //Check if the device exists
            char devicename[MAX_DEVICE_NAME];
            strcpy(devicename, syscall.arg1);
            //int deviceInStorage = 1;
            long long readspeed;
            double readtime;
            int bytes = convertToInt(syscall.arg2);
            for (int j = 0; j < deviceStorage->num_devices; j++){
                if (strcmp(deviceStorage->devices[j].devicename, syscall.arg1) == 0) {
                    //deviceInStorage = 0;
                    readspeed = deviceStorage->devices[j].readspeed;
                    readtime = deviceCalculator(readspeed, bytes);
                    break;
                }
            }
            printf("device %s will read %d bytes in %f usecs.\n", devicename, bytes, readtime);
            syscall.elapsed_time += readtime;
        } else if (strcmp(syscall.syscall, "write") == 0) {
            printf("Executing syscall %s\n", syscall.syscall);
            //syscall.arg1 is the device name, syscall.arg2 is the number of bytes to write
            //Check if the device exists
            char devicename[MAX_DEVICE_NAME];
            strcpy(devicename, syscall.arg1);
            //int deviceInStorage = 1;
            long long writespeed;
            double writetime;
            int bytes = convertToInt(syscall.arg2);
            for (int j = 0; j < deviceStorage->num_devices; j++){
                if (strcmp(deviceStorage->devices[j].devicename, syscall.arg1) == 0) {
                    //deviceInStorage = 0;
                    writespeed = deviceStorage->devices[j].writespeed;
                    writetime = deviceCalculator(writespeed, bytes);
                    break;
                }
            }
            printf("device %s will write %d bytes in %f usecs.\n", devicename, bytes, writetime);
            syscall.elapsed_time += writetime;




        } else if (strcmp(syscall.syscall, "spawn") == 0) {


            printf("Executing syscall %s\n", syscall.syscall);
            //syscall.arg1 is the command name

            char commandName[MAX_COMMAND_NAME];
            strcpy(commandName, syscall.arg1);
            int commandFound = 0;
            for (int j = 0; j < commandStorage->num_commands; j++){
                if (strcmp(commandStorage->commands[j].name, commandName) == 0){
                    Process spawnedProcess;
                    initProcess(&spawnedProcess);
                    spawnedProcess.command = commandStorage->commands[j];
                    spawnedProcess.state = READY;
                    spawnedProcess.id = process->id + 1;
                    spawnedProcess.parent_id = process->id;
                    enqueue(&readyQueue, spawnedProcess);
                    printf("Moving Process ID %d with name %s to ready queue\n", spawnedProcess.id, spawnedProcess.command.name);
                    commandFound = 1;
                    break;
                }
            }
            if (!commandFound){
                printf("%sCommand %s not found%s\n", COLOUR_RED, commandName, COLOUR_NORMAL);
            }
           
            //Check if the command exists
        } else if (strcmp(syscall.syscall, "wait") == 0) {
            printf("Executing syscall %s\n", syscall.syscall);
            if (!isEmpty(&runningQueue)) {
                process->state = BLOCKED;
                process->waiting_process_id = runningQueue.processes[0].id;
                printf("Moving Process ID %d with name %s to blocked queue\n", process->id, process->command.name);
                printf("Waiting for process ID %d\n", process->waiting_process_id);
                //Stop executing the process
                process->current_call = i + 1;
                break;
            }
            else {
                printf("No process to wait for\n");
            }
        } else if (strcmp(syscall.syscall, "exit") == 0) {
            printf("Executing syscall %s\n", syscall.syscall);
            //Move the process to the exit queue
            process->state = EXIT;
            Process process = dequeue(&runningQueue);
            enqueue(&exitQueue, process);
            //dequeue from running queue
            printf("Moving Process ID %d with name %s to exit queue\n", process.id, process.command.name);
            // break out of the loop - no more syscalls to execute, even though it should technically be the last syscall anyway
            break;
        }
    }
}

void execute_commands(CommandStorage *commandStorage, DeviceStorage *deviceStorage)
{
    //Create a queue for each state


    //Initialize the queues
    initializeQueue(&readyQueue);
    initializeQueue(&runningQueue);
    initializeQueue(&blockedQueue);
    initializeQueue(&exitQueue);

    //Create an initial process for each command add it to the ready queue
    for (int i = 0; i < commandStorage->num_commands; i++) {
        Process process;
        process.command = commandStorage->commands[i];
        process.state = READY;
        process.id = i;
        process.parent_id = -1;
        enqueue(&readyQueue, process);
        printf("Adding Process ID %d with name %s to ready queue\n", i, process.command.name);
    

    // While there are still processes in the ready queue execute them and move them to the running queue
    while (!isEmpty(&readyQueue) || !isEmpty(&runningQueue)) {
        if (!isEmpty(&readyQueue)) {
            Process process = dequeue(&readyQueue);
            process.state = RUNNING;
            enqueue(&runningQueue, process);
            printf("Moving Process ID %d with name %s to running queue\n", process.id, process.command.name);
            executeSyscall(&process, deviceStorage, commandStorage);
        }
        if (!isEmpty(&blockedQueue)) {
            for (int i = 0; i < blockedQueue.num_processes; i++) {
                Process blockedProcess = blockedQueue.processes[i];
                int waiting_process_id = blockedProcess.waiting_process_id;
                int found = 0;
                for (int j = 0; j < runningQueue.num_processes; j++) {
                    if (runningQueue.processes[j].id == waiting_process_id) {
                        found = 1;
                        break;
                    }
                }
                if (found) {
                    // Remove the process from the blocked queue
                    blockedProcess = dequeue(&blockedQueue);
                    blockedProcess.state = READY;
                    enqueue(&readyQueue, blockedProcess);
                    printf("Moving Process ID %d with name %s to ready queue\n", blockedProcess.id, blockedProcess.command.name);
                    i--; // Decrement i as the queue length has reduced
                }
            }
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
    printf("measurements  %i  %i\n", 0, 0);

    exit(EXIT_SUCCESS);
}

//  vim: ts=8 sw=4

