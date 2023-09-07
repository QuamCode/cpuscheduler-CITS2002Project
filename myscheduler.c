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

typedef struct{
    char commandname[MAX_COMMAND_NAME];
} CommandInfo;

void read_sysconfig(char argv0[], char filename[])
{

    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("unable to open '%s'\n", filename);
        exit(EXIT_FAILURE);
    }
    char buffer[9999];
    int ndevices = 0;  // Initialize the device count
    
    DeviceInfo devices[MAX_DEVICES];
    int deviceIndex = 0;

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
                    // Store the device info
                    strcpy(devices[deviceIndex].devicename, devicename);
               //     printf("%s", devicename);
                    // store the read and write speeds
                    devices[deviceIndex].readspeed = readspeed;
                    devices[deviceIndex].writespeed = writespeed;
                    deviceIndex++;
                }
            } else if (strcmp(token, "timequantum") == 0) {
                int timequantum;
                if (sscanf(buffer, "%*s %d", &timequantum) == 1) {
                    printf("Time Quantum: %dusec\n", timequantum);
                }
            }
        }

    }

    // Print the number of devices
    printf("Number of Devices: %d\n", deviceIndex);
// Print the device info
    for (int i = 0; i < deviceIndex; i++) {
        printf("Device %d: %s, %lldBps, %lldBps\n", i, devices[i].devicename, devices[i].readspeed, devices[i].writespeed);
    }

    fclose(fp);
}


void read_commands(char argv0[], char filename[])
{
    // open the commands file
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("unable to open '%s'\n", filename);
        exit(EXIT_FAILURE);
    }
    char buffer[9999];
    char command[MAX_COMMAND_NAME];
    CommandInfo commandInfo;

    while (fgets(buffer, sizeof buffer, fp) != NULL) {
        // ignore comments and new line
        if (buffer[0] == CHAR_COMMENT || buffer[0] == '\n') continue;
        
        if (buffer[0] == '\t'){
            char token[MAX_COMMAND_NAME];
            sscanf(buffer, "%*s %s", token);
            printf("System Call: %s\n", token);
        } else {
            if (sscanf(buffer, "%s", command) == 1){
                char token[MAX_COMMAND_NAME];
                sscanf(buffer, "%s", token);
                printf("Command: %s\n", token);
                }
            }

        if (sscanf(buffer, "%s", command) == 1){
            
        }


    
        // create an array of commands per line
        char cmdline[MAX_COMMANDS]; // 20
        for (int i = 0; i < sizeof buffer; i++) {
            int cmdindex = 0;
            if (buffer[i] == '\t') continue;
            cmdline[cmdindex] += buffer[i];
            cmdindex += 1;
        }
        char token[MAX_COMMAND_NAME];


        // if(buffer[0] == '\t'){
        //     printf("System Call: ");
        //     if (sscanf(buffer, "%s", token) == 1) {
        //         printf("%s", token);
        //         printf("\n");
        //     }
        // }
        //printf("COMMAND FILE \n");
        //printf(buffer,"%s \n");
        //printf(cmdline,"%s \n");
    }
    // finished with the file
    fclose(fp);
}

//  ----------------------------------------------------------------------

void execute_commands(void)
{
    /*
    if ("wait" in line) {
        // wait
    }
    else if ...
    */
}


//  ----------------------------------------------------------------------

int main(int argc, char *argv[])
{
//  ENSURE THAT WE HAVE THE CORRECT NUMBER OF COMMAND-LINE ARGUMENTS
    if(argc != 3) {
        printf("Usage: %s sysconfig-file command-file\n", argv[0]);
        exit(EXIT_FAILURE);
    }

//  READ THE SYSTEM CONFIGURATION FILE
    read_sysconfig(argv[0], argv[1]);

//  READ THE COMMAND FILE
    read_commands(argv[0], argv[2]);

//  EXECUTE COMMANDS, STARTING AT FIRST IN command-file, UNTIL NONE REMAIN
 //   execute_commands();

//  PRINT THE PROGRAM'S RESULTS
    printf("measurements  %i  %i\n", 0, 0);

    exit(EXIT_SUCCESS);
}

//  vim: ts=8 sw=4
