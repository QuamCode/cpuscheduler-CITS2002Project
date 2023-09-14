#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_DEVICES                     4
#define MAX_DEVICE_NAME                 20
#define MAX_COMMANDS                    10
#define MAX_COMMAND_NAME                20
#define MAX_SYSCALLS_PER_PROCESS        40
#define MAX_RUNNING_PROCESSES           50

#define DEFAULT_TIME_QUANTUM            100

#define TIME_CONTEXT_SWITCH             5
#define TIME_CORE_STATE_TRANSITIONS     10
#define TIME_ACQUIRE_BUS                20

#define CHAR_COMMENT '#'

struct DeviceInfo {
    char devicename[MAX_DEVICE_NAME];
    long long readspeed;
    long long writespeed;
};
struct CommandLine {
    char commandname[MAX_COMMAND_NAME];
    int commandtime;
};

int convertToInt(char value[]) {
    int newVal;
    newVal = atoi(value);
    return newVal;
}

int usecCalculator(int usecs[], int size) {
    int total = 0;
    for (int i = 0; i < size; i++) {
        total += usecs[i];
    }
    return total;
}

double deviceCalculator(int speed, int amount) {
    double f_speed = (float)speed;
    double f_amount = (float)amount;
    double secondsTaken = f_amount/f_speed;
    // 1 second = 1000000 microseconds
    int usecsTaken = secondsTaken * 1000000;
    return usecsTaken;
}

//  ----------------------------------------------------------------------

int main(int argcount, char *argvalue[]) {
    /* using:
        #            devicename   readspeed      writespeed
        device       terminal     10Bps          3000000Bps
        ...
        cal
            80usecs	write   terminal 2000B
            90usecs	exit
    */
    /* SYSCONFIG */
    char device[] = "usb3.1";
    // bps
    int readspeed = convertToInt("10Bps");
    int writespeed = convertToInt("3000000Bps");
    printf("bps conversions: %d %d\n", readspeed, writespeed);
    /* COMMANDS */
    // usecs
    char *usecsArrayStrings[] = {"80usecs"};
    int arraySize = sizeof(usecsArrayStrings) / sizeof(usecsArrayStrings[0]);
    int usecsArray[arraySize];
    printf("usec conversions: ");
    for (int i = 0; i < arraySize; i++) {
        usecsArray[i] = convertToInt(usecsArrayStrings[i]);
        printf("%d ", usecsArray[i]);
    }
    printf("\n");
    int totalUsecs = usecCalculator(usecsArray, arraySize);
    printf("total usecs taken: %d\n", totalUsecs);
    // b
    int bamount = convertToInt("2000B");
    // calculations
    int something = deviceCalculator(writespeed, bamount);
    printf("device %s will read %d bytes in %d usecs.\n", device, bamount, something);

    // end of program
    exit(EXIT_SUCCESS);
    return 0;
}