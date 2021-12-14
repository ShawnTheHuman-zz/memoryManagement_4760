/*
	Shawn Brown

	project 6 - CS4760

	sysclock.h

*/
#ifndef SYSCLOCK_H
#define SYSCLOCK_H


#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <time.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <sys/msg.h>
#include <string.h>
#include <sys/wait.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>
#include <getopt.h>
#include <signal.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>
#include <stdbool.h>



#define SECOND_TIMER 100
#define PROCESS_MAX 18

// Structure for memory manager
typedef struct memory_manager{
        pid_t pid;
        int resource_Marker;
        int table_size[32];

} memory_manager;

// stores up to 18 processes in array of memory manager
// objects
struct memory_manager *resource_array_size[18];
struct memory_manager *(*resource_array_ptr)[] = &resource_array_size;

// Globals

extern int limit;
extern int percentage;


int get_random(int,int);
bool rand_prob(float);
pid_t spawn_child(char *, char *, char*, char*, char*, char*);
void shm_get(int *, int *, int*, key_t , key_t , key_t );
void arg_manager(char *, char *, char *, char *, char *, char *, int, int, int, int, int, int);
void shm_at(unsigned int **, unsigned int **, sem_t **, memory_manager **, int, int, int);
void gen_keys(key_t*,key_t*,key_t*);
void usage();


#endif