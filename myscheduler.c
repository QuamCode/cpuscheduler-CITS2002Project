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
//  CPU EXECUTION QUEUE

//  Define the states
typedef enum {NEW, READY, RUNNING, BLOCKEDm, EXIT} State;

//Create a struct for the process
typedef struct {
    Command command;
    State state;
} Process;

//Create a struct for the queue
typedef struct {
    Process processes[MAX_RUNNING_PROCESSES];
    int num_processes;
} Queue;


//  ----------------------------------------------------------------------











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
        printf("Error: Maximum number of commands reached.\n");
    }
}

void addDevice(DeviceStorage *storage, DeviceInfo device) {
    if (storage->num_devices < MAX_DEVICES) {
        storage->devices[storage->num_devices] = device;
        storage->num_devices++;
    } else {
        printf("Error: Maximum number of devices reached.\n");
    }
}




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
                printf("Error parsing line: %s\n", buffer);
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

void execute_commands(CommandStorage *commandStorage, DeviceStorage *deviceStorage)
{

    //Create a queue for each state
    Queue readyQueue;
    Queue runningQueue;
    Queue blockedQueue;
    Queue exitQueue;

    //Initialize the queues
    readyQueue.num_processes = 0;
    runningQueue.num_processes = 0;
    blockedQueue.num_processes = 0;
    exitQueue.num_processes = 0;

    //Create an initial process for each command
    for (int i = 0; i < commandStorage->num_commands; i++) {
        Process process;
        process.command = commandStorage->commands[i];
        process.state = READY;
        readyQueue.processes[newQueue.num_processes++] = process;
    }

    //Execute from the first command
    while (readyQueue.num_processes > 0) {
        //Get the first process in the new queue
        Process process = readyQueue.processes[0];
        //Set the state to ready
        process.state = RUNNING;
        //Add the process to the ready queue
        runningQueue.processes[readyQueue.num_processes++] = process;
        //Remove the process from the new queue
        for (int i = 0; i < readyQueue.num_processes; i++) {
            readyQueue.processes[i] = newQueue.processes[i+1];
        }
        readyQueue.num_processes--;

        //Execute the system calls
        while (process.command.num_syscalls > 0) {
            Syscall syscall = process.command.syscalls[0];
            printf("Syscall: %s\n", syscall.syscall);
            if (strcmp(syscall.syscall, "sleep") == 0) {
                // Sleep
                        process.state = BLOCKED;
            } else if (strcmp(syscall.syscall, "write") == 0) {
                // Write to device
                        process.state = BLOCKED;
                    
            } else if (strcmp(syscall.syscall, "read") == 0) {
                // Read from device
                    process.state = BLOCKED;
                
            } else if (strcmp(syscall.syscall, "spawn") == 0) {
                // Spawn
                printf("Spawning\n");
                process.state = BLOCKED;
            } else if (strcmp(syscall.syscall, "wait") == 0) {
                // Wait
                printf("Waiting\n");
                process.state = BLOCKED;
            }
            else if (strcmp(syscall.syscall, "exit") == 0) {
                // Exit
                printf("Exiting\n");
                process.command.num_syscalls = 0;
                process.state = BLOCKED;
            } else {
                printf("Error: Unknown syscall %s\n", syscall.syscall);
            }
    }
}
}

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

