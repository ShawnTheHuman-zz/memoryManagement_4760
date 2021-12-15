/*
	Shawn Brown
	project 6 - CS4760
	
	oss.cpp

	main driver for project
*/


#include "oss.h"
#include "sysclock.h"



// Strucutre for messages
struct message
{
	long msgString;
	char msgChar[100];
} message;

static int seg_fault_counter = 0;


// stores up to 18 processes in array of memory manager
// objects
struct memory_manager *resource_array_size[18];
struct memory_manager *(*resource_array_ptr)[] = &resource_array_size;

// Globals
int alrm, processCount, frameTablePos = 0;
int setArr[20] = {0};
void rand_fork(unsigned int *seconds, unsigned int *nanoseconds, unsigned int *forkTimeSeconds, unsigned int *);
int checkArray(int *placementMarker);

void seg_signal(int signal, siginfo_t *si, void *arg); 


// Main
int main(int argc, char *argv[])
{
	int opt;
	processCount = PROCESS_MAX; 
	// parse command line options
	while ((opt = getopt(argc, argv, "hp:")) != -1)
	{
		switch (opt)
		{
		case 'h':
			usage();
			exit(0);
		case 'p':
			if (strspn(argv[2], "0123456789") != strlen(argv[2]))
			{
				printf("OSS: ERROR: Entered value for -p not an integer\n\n");
				usage();
				exit(0);
			}
			processCount = atoi(optarg);
			if (processCount < 1 || processCount > 18)
			{
				printf("OSS: ERROR: unput must be 18: defaulting to 18\n");
				processCount = PROCESS_MAX;
			}
			break;
		default:
			perror("OSS: ERROR: Invalid argument");
			usage();
			exit(1);
		}
	}

	if ((argc == 1) || (strcmp(argv[1], "-p") != 0))
	{
		perror("OSS: ERROR: -p requires an argument greater than zero\n\n");
		usage();
		return 0;
	}

	printf("oss: running with %d processes.\n", processCount);

	// set up signal to catch segmentation faults
	struct sigaction sa;
	memset(&sa, 0, sizeof(struct sigaction));
	sigemptyset(&sa.sa_mask);
	sa.sa_sigaction = seg_signal;
	sa.sa_flags = SA_SIGINFO;
	sigaction(SIGSEGV, &sa, NULL);

	// For shared memory info
	char childMsg[20], ch;
	char requestType[20];
	char sharedPositionMem[10];
	char sharedPercentageMem[10];
	char sharedTimeMem[10];
	char rscShrdMem[10];
	char sharedSemMem[10];
	char sharedLimitMem[10];

	int address;
	int forked = 0;
	int lines = 0;
	int frameLoop = 0;
	int pagefault = 0;
	int requests = 0;
	int initialFork = 0;
	int tempPid = 0;
	int status;

	// shared clock data
	unsigned int *seconds = 0;
	unsigned int *nanoseconds = 0;
	unsigned int forkTimeSeconds = 0;
	unsigned int forkTimeNanoseconds = 0;
	unsigned int accessSpeed = 0;


	char *fileName = "logfile";
	FILE *outfile = fopen(fileName, "w");
	freopen("logfile", "a", outfile);

	srand(time(NULL));


	int percentage = 50, maxProcL = 900;

	// 2d array to store the frame table
	int frameTable[256][3] = {{0}};


	// key for message queue and initialize keys for
	// shared memory items 
	key_t msgKey = ftok(".", 432820);
	key_t clock_key = 0, rscKey = 0, semKey = 0;

	int msgid = msgget(msgKey, 0666 | IPC_CREAT);
	int timeid = 0, rscID = 0, semid = 0, placementMarker = 0;

	// initialize shared memory structurs
	memory_manager *resource_ptr = NULL;
	sem_t *semPtr = NULL;

	// generate keys and attach to shared memory
	gen_keys(&clock_key, &semKey, &rscKey);
	shm_get(&timeid, &semid, &rscID, clock_key, semKey, rscKey);
	shm_at(&seconds, &nanoseconds, &semPtr, &resource_ptr, timeid, semid, rscID);

	// performance data initialization
	double pageFaults = 0, memoryAccesses = 0, memoryAccessesPerSecond = 0;
	float address_request = 0;
	

	// initialize alarm for kill timer
	signal(SIGALRM, killTimer);
	alarm(2);


	// main loop
	do
	{
		if (initialFork == 0)
		{
			rand_fork(seconds, nanoseconds, &forkTimeSeconds, &forkTimeNanoseconds);
			initialFork = 1;
			fprintf(outfile, "OSS: Fork Time starts at %d:%d\n", forkTimeSeconds, forkTimeNanoseconds);
		}

		*nanoseconds += 50000;

		if (*nanoseconds >= 1000000000)
		{
			*seconds += 1;
			*nanoseconds = 0;
			memoryAccessesPerSecond = (memoryAccesses / *seconds);
		}

		if (((*seconds == forkTimeSeconds) && (*nanoseconds >= forkTimeNanoseconds)) || (*seconds > forkTimeSeconds))
		{
			if (checkArray(&placementMarker) == 1)
			{
				forked++;
				initialFork = 0;

				fprintf(outfile, "OSS: Forked at time %d:%d \n", *seconds, *nanoseconds);

				// copy arguments into shared memory
				arg_manager(sharedTimeMem, sharedSemMem, sharedPositionMem, rscShrdMem, sharedLimitMem, sharedPercentageMem, timeid, semid, rscID, placementMarker, maxProcL, percentage);
				

				pid_t childPid = spawn_child(sharedTimeMem, sharedSemMem, sharedPositionMem, rscShrdMem, sharedLimitMem, sharedPercentageMem);
				
				resource_array_size[placementMarker] = malloc(sizeof(struct memory_manager));
				
				(*resource_array_ptr)[placementMarker]->pid = childPid;
				
				fprintf(outfile, "OSS: Child %d spawned with PID %d at time %d:%d\n", placementMarker, childPid, *seconds, *nanoseconds);

				for (int i = 0; i < 32; i++)
				{
					(*resource_array_ptr)[placementMarker]->table_size[i] = -1;
				}

				(*resource_array_ptr)[placementMarker]->resource_Marker = 1;
			}
		}

		for (int i = 0; i < processCount; i++)
		{
			if (setArr[i] == 1)
			{
				tempPid = (*resource_array_ptr)[i]->pid;

				if ((msgrcv(msgid, &message, sizeof(message) - sizeof(long), tempPid, IPC_NOWAIT | MSG_NOERROR)) > 0)
				{
					/* check message to see if the page is going to be written
					   or read and print to log  */
					if (atoi(message.msgChar) != 99999)
					{
						fprintf(outfile, "OSS: P%d requesting read of address %d to ", i, atoi(message.msgChar));
						strcpy(childMsg, strtok(message.msgChar, " "));
						address = atoi(childMsg);
						strcpy(requestType, strtok(NULL, " "));
						
						//  depending on type from message it prints read or write 
						if (atoi(requestType) == 0)
						{
							fprintf(outfile, "be read at time %d:%d\n", *seconds, *nanoseconds);
						}
						else
						{
							fprintf(outfile, "be written at time %d:%d\n", *seconds, *nanoseconds);
						}

						address_request = (atoi(childMsg)) / 1000;
						address_request = (int)(floor(address_request));

						// If page table position is empty or is no longer associated with the child request address -- assign to frame table
						if ((*resource_array_ptr)[i]->table_size[(int)address_request] == -1 || frameTable[(*resource_array_ptr)[i]->table_size[(int)address_request]][0] != (*resource_array_ptr)[i]->pid)
						{
							frameLoop = 0;
							
							// Check for first available frame
							while (frameTable[frameTablePos][0] != 0 && frameLoop < 255)
							{
								frameTablePos++;
								frameLoop++;

								if (frameTablePos == 256)
									frameTablePos = 0;
								if (frameLoop == 255)
									pagefault = 1;
							}

							// if a page fault is found, print to the screen
							if (pagefault == 1)
							{
								pageFaults++;
								fprintf(outfile, "OSS: Address %d is not in a frame, page fault\n", address);

								while (frameTable[frameTablePos][1] != 0)
								{									  // Check for second frame if it exists
									frameTable[frameTablePos][1] = 0; // Set to 0 if it was 1
									frameTablePos++;				  // Move to next frame

									if (frameTablePos == 256)
										frameTablePos = 0;
								}

								if (frameTable[frameTablePos][1] == 0)
								{
									memoryAccesses++;
									fprintf(outfile, "OSS: Clearing frame %d and swapping in P%d page %d at time %d:%d\n",
											frameTablePos, i, (int)address_request, *seconds, *nanoseconds);

									// New page frame goes in this section
									(*resource_array_ptr)[i]->table_size[(int)address_request] = frameTablePos;
									frameTable[frameTablePos][0] = (*resource_array_ptr)[i]->pid;
									frameTable[frameTablePos][2] = atoi(requestType);
									fprintf(outfile, "OSS: Address %d in frame %d giving data to P%d at time %d:%d\n",
											address, frameTablePos, i, *seconds, *nanoseconds);
									frameTablePos++; // Our clock increments in sec/nanosec
									if (frameTablePos == 256)
										frameTablePos = 0;
									requests++;
								}
								
								accessSpeed += 15000000;
								*nanoseconds += 15000000;
								fprintf(outfile, "OSS: Dirty bit is set to %d and clock is incremented 15ms\n", atoi(requestType));
							}
							// If an empty frame is found
							else
							{
								memoryAccesses++;
								(*resource_array_ptr)[i]->table_size[(int)address_request] = frameTablePos;
								frameTable[frameTablePos][0] = (*resource_array_ptr)[i]->pid;
								frameTable[frameTablePos][1] = 0; // Resources is now clear
								frameTable[frameTablePos][2] = atoi(requestType);
								fprintf(outfile, "OSS: Address %d in frame %d giving data to P%d at time %d:%d\n",
										address, frameTablePos, i, *seconds, *nanoseconds);
								frameTablePos++; // Advance clock.
								if (frameTablePos == 256)
								{
									frameTablePos = 0;
								}
								accessSpeed += 10000000;
								*nanoseconds += 10000000;
								requests++;
								fprintf(outfile, "OSS: Dirty bit is set to %d and adding time to the clock\n", atoi(requestType));
							}
						}
						// sets reference and dirty bits based on request type, increments clock
						// and requests then prints to log
						else
						{
							memoryAccesses++;
							frameTable[(*resource_array_ptr)[i]->table_size[(int)address_request]][1] = 1;				   // Referenced bit set
							frameTable[(*resource_array_ptr)[i]->table_size[(int)address_request]][2] = atoi(requestType); // Dirty Bit is set
							*nanoseconds += 10000000;
							accessSpeed += 10000000;
							requests++;
							fprintf(outfile, "OSS: Dirty bit is set to %d, additional time added to the clock\n", atoi(requestType));
						}

						message.msgString = ((*resource_array_ptr)[i]->pid + 118);
						sprintf(message.msgChar, "wakey");
						msgsnd(msgid, &message, sizeof(message) - sizeof(long), 0);
					}

					// checks message to see if process is finished, clears frames if so. 
					else if (atoi(message.msgChar) == 99999)
					{
						setArr[i] = 0;
						message.msgString = ((*resource_array_ptr)[i]->pid + 118);
						fprintf(outfile, "OSS: P%d is complete -- clearing frames: ", i);

						for (int j = 0; j < 32; j++)
						{
							if ((*resource_array_ptr)[i]->table_size[j] != -1 
								&& frameTable[(*resource_array_ptr)[i]->table_size[j]] 
								== (*resource_array_ptr)[i]->table_size[j])
							{
								fprintf(outfile, "%d, ", j);
								frameTable[(*resource_array_ptr)[i]->table_size[j]][0] = 0;
								frameTable[(*resource_array_ptr)[i]->table_size[j]][1] = 0;
								frameTable[(*resource_array_ptr)[i]->table_size[j]][2] = 0;
								(*resource_array_ptr)[i]->table_size[j] = -1;
							}
						}

						fprintf(outfile, "\n");
						msgsnd(msgid, &message, sizeof(message) - sizeof(long), 0);
						waitpid(((*resource_array_ptr)[i]->pid), &status, 0);
						free(resource_array_size[i]);
					}
				}
			}
		}

		while ((ch = fgetc(outfile)) != EOF)
		{
			if (ch == '\n')
			{
				lines++;
			}
		}
		// Stop printing and close file if line number exceeds 100000
		if (lines >= 100000)
		{
			fprintf(outfile, "\nOSS: Total line number has exceeded 100000 -- now closing file\n");
			fclose(outfile);
		}

	} while ((*seconds < SECOND_TIMER + 10000) && alrm == 0 && forked < 100);

	// Print final statistics
	fprintf(outfile, "\nOSS: Program complete\n");
	fprintf(outfile, "\t----- STATISTICS -----\n\tNumber of memory accesses ser second: %f\n\tNumber of page faults per memory access: %f\n\tAverage Access Speed in nanosec: %f\n\n",
					memoryAccessesPerSecond, pageFaults / memoryAccesses, floor(accessSpeed / memoryAccesses));

	// Cleanup and close output file
	fclose(fp);
	shmdt(seconds);
	shmdt(semPtr);
	
	shmdt(resource_ptr);
	msgctl(msgid, IPC_RMID, NULL);
	shmctl(msgid, IPC_RMID, NULL);
	shmctl(rscID, IPC_RMID, NULL);
	shmctl(timeid, IPC_RMID, NULL);
	shmctl(semid, IPC_RMID, NULL);

	printf("OSS: Terminating program\n");
	kill(0, SIGTERM);

	return 0;
}

// prints usage information
void usage()
{
	printf("---------- USAGE ----------\n");
	printf("./oss [-h] [-p]\n");
	printf("-h\tDisplays usage message (optional)\n");
	printf("-p\tSpecify number between 1 and 20\n");
	printf("---------------------------\n");
}

// sends kill signal
void killTimer(int signal)
{
	alrm = 1;
}

int checkArray(int *placementMarker)
{
	for (int i = 0; i < processCount; i++)
	{
		if (setArr[i] == 0)
		{
			setArr[i] = 1;
			*placementMarker = i;
			return 1;
		}
	}
	return 0;
}


// custom signal for seg faults
void seg_signal(int signal, siginfo_t *si, void *arg)
{

	fprintf(stderr, "Caught segfault at address %p\n", si->si_addr);
	seg_fault_counter++;

	kill(0, SIGTERM);
}
// fork a user process at random times between 1 and 500 milliseconds based on 
// simulated clock. 
void rand_fork(unsigned int *seconds, unsigned int *nanoseconds, unsigned int *forkTimeSeconds, unsigned int *forkTimeNanoseconds)
{
	unsigned int random = rand() % 500000000;
	*forkTimeNanoseconds = 0;
	*forkTimeSeconds = 0;

	if ((random + nanoseconds[0]) >= 1000000000)
	{
		*forkTimeSeconds += 1;
		*forkTimeNanoseconds = (random + *nanoseconds) - 1000000000;
	}
	else
	{
		*forkTimeNanoseconds = random + *nanoseconds;
	}

	*forkTimeSeconds = *seconds;
}