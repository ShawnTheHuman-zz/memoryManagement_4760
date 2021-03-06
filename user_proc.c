/*
    Shawn Brown

    project 6 - CS4760

    user_proc.cpp

*/


#include "sysclock.h"

// message struct definition
struct message
{
    long msgString;
    char msgChar[100];
} message;

// Main
int main(int argc, char *argv[])
{

    int complete = 0, request = 0;
    srand(getpid());

    // ftok to generate key for message and setting up
    // and gets msg key
    key_t msgKey = ftok(".", 432820);
    int msgid = msgget(msgKey, 0666 | IPC_CREAT);

    // storing aruguments from exec in variables
    int clockid = atoi(argv[1]);
    int semid = atoi(argv[2]);
    int resource_id = atoi(argv[4]);
    int limit = atoi(argv[5]);
    int percentage = atoi(argv[6]);
    int event = 0;

    pid_t pid = getpid();

    // initialize pointers for semaphore and mem manager
    // objects

    sem_t *semPtr;
    memory_manager *resource_ptr;

    // initialize clock data to zero
    unsigned int *seconds = 0,
                 *nanoseconds = 0,
                  eventTimeSeconds = 0,
                  eventTimeNanoseconds = 0,
                  requests = 0;

    // attach everything to shared memory
    shm_at(&seconds, &nanoseconds, &semPtr, &resource_ptr, clockid, semid, resource_id);
    message.msgString = pid;
    message.msgString = 12345;

    // generate random time for clock
    randomTimer(seconds, nanoseconds, &eventTimeSeconds, &eventTimeNanoseconds);

    // use random timer based on the clock to determine the status of the 
    // process and send it to oss
    while (complete == 0)
    {
        if ((*seconds == eventTimeSeconds && *nanoseconds >= eventTimeNanoseconds) || *seconds > eventTimeSeconds)
        {
            event = rand() % 99;
            request = rand() % 32001;
            requests++;
            randomTimer(seconds, nanoseconds, &eventTimeSeconds, &eventTimeNanoseconds);

            // sends clear message
            if (requests == limit && event < 75)
            {
                message.msgString = (int)pid;
                sprintf(message.msgChar, "%d", 99999);
                msgsnd(msgid, &message, sizeof(message) - sizeof(long), 0);
                msgrcv(msgid, &message, sizeof(message) - sizeof(long), (pid + 118), 0);
                complete = 1;
            }

            // sends message for the page to be written
            else if (event < percentage)
            {
                message.msgString = (int)pid;
                sprintf(message.msgChar, "%d %d", request, 0);
                msgsnd(msgid, &message, sizeof(message) - sizeof(long), 0);
                msgrcv(msgid, &message, sizeof(message) - sizeof(long), (pid + 118), 0);
            }

            // sends message for page to be read 
            else if (event >= (99 - percentage))
            {
                message.msgString = (int)pid;
                sprintf(message.msgChar, "%d %d", request, 1);
                msgsnd(msgid, &message, sizeof(message) - sizeof(long), 0);
                msgrcv(msgid, &message, sizeof(message) - sizeof(long), (pid + 118), 0);
            }
        }
    }

    // detaches all shared memory
    shmdt(seconds);
    shmdt(semPtr);
    shmdt(resource_ptr);
    shmctl(msgid, IPC_RMID, NULL);
    exit(0);
}

// Generate random times between 1 and 500 milliseconds for
// forking new child processes
void randomTimer(unsigned int *seconds, unsigned int *nanoseconds, unsigned int *eventTimeSeconds, unsigned int *eventTimeNanoseconds)
{
    unsigned int r = rand() % 500000000;
    *eventTimeNanoseconds = 0;
    *eventTimeSeconds = 0;
    if ((r + *nanoseconds) >= 1000000000)
    {
        *eventTimeSeconds += 1;
        *eventTimeNanoseconds = (r + *nanoseconds) - 1000000000;
    }
    else
    {
        *eventTimeNanoseconds = r + *nanoseconds;
    }

    *eventTimeSeconds = *seconds;
}