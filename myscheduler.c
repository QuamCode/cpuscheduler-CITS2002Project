#include <stdio.h>
#include <stdlib.h>
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


//  ----------------------------------------------------------------------

#define CHAR_COMMENT                    '#'

void read_sysconfig(char argv0[], char filename[])
{
    // create array of devices
    char devices[99];
	// per each item in devices array:
    char name[99];
    int read_speed;
    int write_speed;
	// and then add to devices array
    // store number of devices
	int ndevices = 0;

    //Read the sysconfig file, which is a tab seperated file, ignoring lines that start with a #pragma endregion
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("unable to open '%s'\n", filename);
        exit(EXIT_FAILURE);
    }
    char buffer[9999];
    //read each line of the file, ignore lines that start with #
    //record an integer for each line that starts with device
    //Create an array that assigns the device to the integer with the name from the second tsv column

    while (fgets(buffer, sizeof buffer, fp) != NULL) {
        if (buffer[0] == CHAR_COMMENT) continue;
        char name[99];
        int read_speed;
        int write_speed;
        sscanf(buffer, "%s %i %i", name, &read_speed, &write_speed);
        printf("%s %i %i\n", name, read_speed, write_speed);
    }

    
}

void read_commands(char argv0[], char filename[])
{
    // attempt to open the file for read-only access
    int fd = open(filename, O_RDONLY);
    // catch error if file cannot be opened
    if (fd == -1) {
        printf("unable to open '%s'\n", filename);
        exit(EXIT_FAILURE);
    }
    // create an array to hold file contents
    char buffer[9999];
    size_t got;
    // read file multiple times until end of file
    while((got = read(fd, buffer, sizeof buffer)) > 0) {  
        // for each command call execute_commands();
    }
    // finished with the file
    close(fd);
}

//  ----------------------------------------------------------------------

void execute_commands(void)
{
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
    execute_commands();

//  PRINT THE PROGRAM'S RESULTS
    printf("measurements  %i  %i\n", 0, 0);

    exit(EXIT_SUCCESS);
}

//  vim: ts=8 sw=4
