/*
	Shawn Brown

	project 6 - CS4760

	sysclock.h

*/



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

#define STRUCT_SZ ((sizeof(resource_array_size)/sizeof(resource_array_size[0])) * 18)
#define SEM_SIZE sizeof(int)
#define QUE_SZ 18

#ifndef FALSE
#define FALSE (0)
#endif
#ifndef TRUE
#define TRUE (!FALSE)
#endif

// Structure for memory manager
typedef struct memory_manager{
        pid_t pid;
        int resource_Marker;
        int tableSz[32];

} memory_manager;

// stores up to 18 processes in array of memory manager
// objects
struct memory_manager *resource_array_size[18];
struct memory_manager *(*resource_array_ptr)[] = &resource_array_size;

// Globals

extern int limit;
extern int percentage;


std::string int2str();
std::string float2str();
void print_array();
std::string array_string();
int get_array_value();
std::string print_time();
std::string string_format();
int get_random();
bool rand_prob();
pid_t spawn_child();
void shm_get();
void arg_manager();
void shm_at();
void gen_keys();