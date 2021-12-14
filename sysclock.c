/*
	Shawn Brown

	project 6 - CS4760

	sysclock.cpp

*/


#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <math.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/msg.h> 
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include "sysclock.h"

using namespace std;




// void print_array( const int* arr, const int arrsize, const int cols )
// {
// 	std::cout << "   ";

// 	for( int i( 0 ); i < cols; i++ )
// 		std::cout << "R" << i << " " << std::endl;

// 	for( int i( 0 ); i < arrsize/cols; i++)
// 	{
// 		cout << "P" << i << ( ( i > 9 ) ? " " : "  ");
// 		for( int j( 0 ); j < cols; j++)
// 			cout << arr[i * cols + j] << (( j > 9 ) ? "  " : "  ") << endl;
// 	}

// }

// std::string array_string( const int* arr, const int arrsize, const int cols )
// {
// 	std::string output = "   ";

// 	for( int i( 0 ); i < cols; i++ )
// 		output.append("R" + int2str(i) + " \n");

// 	for( int i( 0 ); i < arrsize/cols; i++)
// 	{
// 		output.append("P" + int2str(i) + (( i > 9) ? " " : "   "));
// 		for( int j( 0 ); j < cols; j++)
// 			output.append(int2str(arr[i * cols + j]) + (( j > 9 ) ? "   " : "  "));
// 		output.append("\n");
// 	}
// 	return output;
// }


int get_array_value(const int* array, const int row, const int col, const int total_cols)
{
	return array[ row * total_cols + col];

}


void set_array_value( int* arr, const int row, const int col, const int total_cols, int new_val)
{
	arr[ row * total_cols + col] = new_val;

}

// std::string print_time(const char* str)
// {
// 	time_t t;
// 	struct tm * _time;
// 	char buffer[10];

// 	time (&t);
// 	_time = localtime ( &t );


// 	strftime( buffer, 80, "%T", _time);

// 	std::string new_time = str;
// 	new_time.append(buffer);
	
// 	return new_time;
// }




/* formats a string regardless of size.  */
// std::string string_format(const std::string fmt, ...)
// {
// 	int size = ((int)fmt.size()) * 2 + 50;
// 	std::string str;
// 	va_list ap;
// 	while (1) {    
// 		str.resize(size);
// 		va_start(ap, fmt);
// 		int n = vsnprintf((char *)str.data(), size, fmt.c_str(), ap);
// 		va_end(ap);
// 		if (n > -1 && n < size) 
// 		{ 
// 			str.resize(n);
// 			return str;
// 		}
// 		if (n > -1)  
// 			size = n + 1; 
// 		else
// 			size *= 2;
// 	}
// 	return str;
// }




// void write_log(std::string input, std::string filename)
// {
// 	std::cout << input.c_str() << std::endl;


//     std::ofstream logFile (filename.c_str(), 
//             std::ofstream::out | std::ofstream::app);
//     if (logFile.is_open())
//     {
//         logFile << input.c_str() << std::endl;
//         logFile.close();
//     }
//     else
//     {
//         perror("Unable to write to log file");
//     }
// }

// /* second writer for verbose mode  */
// void write_log( std::string sys, int sec, int nano, std::string text, int pid, int index, std::string filename )
// {
// 	std::cout << string_format("%s%.2d %.6d:%.10d\t%s PID %d", sys.c_str(), index, sec, nano, text.c_str(), pid) << std::endl;

// 	std::ofstream logfile ( filename.c_str(), std::ofstream::out | std::ofstream::app );

// 	if( logfile.is_open())
// 	{
// 		logfile << string_format("%s%.2d %.6d:%.10d\t%s PID %d", sys.c_str(), index, sec, nano, text.c_str(), pid) << std::endl;

// 		logfile.close();
// 	}
// 	else
// 	{
// 		perror("ERROR writing to file");
// 	}
// }

int get_random(int MinVal, int MaxVal)
{
    int range = MaxVal-MinVal+1 ;
    return rand()%range + MinVal ;
}


bool rand_prob(float prob)
{
    float fVal = (rand()%1000)/10.0f;
    return fVal < (prob * 100.0f);
}

pid_t pid = 0;
extern int limit;
extern int percentage;


pid_t spawn_child(char * shr_clock, char* sem, char* arr_pos, char* resource_mem, char* limit, char* percentage) {
        if((pid = fork()) == 0) {
                execlp("./user", "./user", shr_clock, sem, arr_pos, resource_mem, limit, percentage, NULL);
        }

        if(pid < 0) {
                printf("Fork Error %s\n", strerror(errno));
        }

        return pid;
};



// function to get shared memory for the clock, semaphore and resource id
// and verifies their creation
void shm_get(int *clock_id, int *sem_id, int*resource_id, key_t clock_key, key_t sem_key, key_t resource_key) {
       *clock_id = shmget(clock_key, (sizeof(unsigned int) * 2), 0666 | IPC_CREAT);
		if(*clock_id == -1) perror("shmem: Error: Failure to create shmem for time");

        *sem_id = shmget(sem_key, (sizeof(unsigned int) * 2), 0666 | IPC_CREAT);
		if(*sem_id == -1) perror("shmem: Error: Failure to create shmem for sem");

        *resource_id = shmget(resource_key, (sizeof(memory_manager) *2),0666|IPC_CREAT);
        if(*resource_id == -1) perror("shmem: Error: Failure to create shmem for res");
};


// function that takes input and prints it to the shared memory arrays
void arg_manager(char *shr_clock, char *sharedSemMem, char *arr_pos, char *resource_mem, char *shr_limit, char *shr_percentage, int clock_id, int sem_id, int resource_id, int placementMarker, int limit, int percentage) {
        snprintf(shr_clock, sizeof(shr_clock)+25, "%d", clock_id);
        snprintf(sharedSemMem, sizeof(sharedSemMem)+25, "%d", sem_id);
        snprintf(arr_pos, sizeof(arr_pos)+25, "%d", placementMarker);
        snprintf(resource_mem, sizeof(resource_mem)+25, "%d", resource_id);
        snprintf(shr_limit, sizeof(limit)+25, "%d", limit);
        snprintf(shr_percentage, sizeof(percentage)+25, "%d", percentage);
};

// function to attach shared memory for clock, semaphore and memory manager object
// and checks if it worked
void shm_at(unsigned int **seconds, unsigned int **nanoseconds, sem_t **semaphore, memory_manager **resource_ptr, int clock_id, int sem_id, int resource_id) {
        *seconds = (unsigned int*)shmat(clock_id, NULL, 0);
        if(**seconds == -1) perror("shmem: Error: Failure to attach shared memory for sec");

        *nanoseconds = *seconds + 1;
        if(**nanoseconds == -1) perror("shmem: Error: Failure to attach shared memory for ns");
        
        *semaphore = (sem_t*)shmat(sem_id, NULL, 0);
        if(*semaphore == (void*)-1) perror("shmem: Error: Failure to attach shared memory for sem");
        
        *resource_ptr = (memory_manager*)shmat(resource_id, NULL, 0);
        if(*resource_ptr == (void*)-1) perror("shmem: Error: Failure to attach shared memory for resPointer");
};

void gen_keys(key_t *resource_key, key_t *sem_key, key_t *clock_key) {
        *resource_key = ftok(".", 6050);
        *sem_key = ftok(".", 5051);
        *clock_key = ftok(".", 3213);
};