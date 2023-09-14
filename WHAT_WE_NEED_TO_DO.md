## What we need to do to finish this.

- [ ] 1. *Time Quantum check after each SysCall to move the current process back to READY*

"executes for a finite time-quantum (after which the process is marked as Ready and is queued until it can run again),"


- [ ] 2. *Blocking Read and Write Processes until complete - READING AND WRITING IS NOT CPU TIME but will add to the OVERALL time*

"at which time the process is marked as Blocked and queued until its I/O request is satisfied"

"The CPU is connected to a number of input/output (I/O) devices of differing speeds, using a single high-speed data-bus. Only a single process can use the data-bus at any one time.

Only a single process can access each I/O device (and the data-bus) at any one time. 

If the data-bus is in use (data is still being transferred) and a second process also needs to access the data-bus, the second process must be queued until the current transfer is complete. 

When a data transfer completes, all waiting (queued) processes are consider to determine which process can next acquire the data-bus. If multiple processes are waiting to acquire the data-bus, the process that has been waiting the longest for the device with the fastest read speed will next acquire the data-bus. Thus, all processes waiting on devices with high read speeds are serviced before any processes that are waiting on devices with lower read speeds."


- [ ] 3. *Implement all Context Switches adding to the execution time*

It takes 5 microseconds to perform a context-switch - to move one process from Ready → Running
It also takes 10 microseconds to move a process from Running → Blocked, Running → Ready, and Blocked → Ready
It takes 20 microseconds for any process to first acquire the data-bus.



it calls exit, RUNNING→EXIT (0 usecs).
it calls any blocking system-call, RUNNING→BLOCKED (10 usecs).
its time-quantum expires, RUNNING→READY (10 usecs). Process that exhaust their time-quantum always return to the READY queue.





- [ ] 4. *Implementing the blocking with Sleep - READING AND WRITING IS NOT CPU TIME but will add to the OVERALL time*

When the process is asleep, move it to blocked queue

- [ ] 5. *CPU idle check (When no processes are running)*

When the CPU is idle (no process or the OS is using the CPU), perform the following in this order:
unblock any sleeping processes
unblock any processes waiting for all their spawned processes (children) to terminate
unblock any completed I/O
commence any pending I/O
commence/resume the next READY process
otherwise the CPU remains idle...


- [ ] 5. *Final Output*

 reporting the total time taken (in microseconds) to complete the emulation (or simulation) of the command sequence in the system (i.e. the time at which there are no more running processes), and the CPU-utilisation (the percentage of the time, as an integer, that the CPU is executing processes)

6. Clean up - add comments, seperate into helper functions, remove unused variables
