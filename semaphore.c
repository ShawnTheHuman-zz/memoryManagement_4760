/*
	Shawn Brown

	project 6 - CS4760

	semaphore.cpp

*/

#include <sys/sem.h>
#include <sys/stat.h>
#include <iostream>
#include "semaphore.h"

/* Semaphore options */
#define PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)


/*

	Semaphore definitions

*/
Semaphore::Semaphore(key_t key, bool Create, int Value)
{
    // If a valid key
    if(key > 0)
    {
        // If Creating a new Key
        if(Create)
        {

            _semid = semget(key, 1, PERMS | IPC_EXCL | IPC_CREAT);

            // If successful, set it's value to Value
            if (_semid > 0)
            {
                semctl(_semid, 0, SETVAL, Value);

                _bCreator = true;
                _initialized = true;
            }
        }
        else
        {

            _semid = semget(key, 1, PERMS);

            _bCreator = false;
            if (_semid > 0)
            {
                // Set as properly initialized
                _initialized = true;
            }
        }
    }
}

Semaphore::~Semaphore()
{
    if(_bCreator && _initialized)
    {
        semctl(_semid, 0, IPC_RMID);

    }
}

void Semaphore::Wait()
{
    sb.sem_num = 0;
    sb.sem_op = -1;
    sb.sem_flg = 0;
    semop(_semid, &sb, 1);
}

void Semaphore::Signal() 
{
    sb.sem_num = 0;
    sb.sem_op = 1;
    sb.sem_flg = 0;
    semop(_semid, &sb, 1);
}
