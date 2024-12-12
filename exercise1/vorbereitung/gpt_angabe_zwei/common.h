#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/mman.h>
#include <signal.h>
#include <time.h>
#include <string.h>


#define SHM_NAME "/server_client_shm"
#define SEM_FREE "/sem_free"
#define SEM_USED "/sem_used"
#define SEM_MUTEX "/sem_mutex"
#define PERMISSIONS 0666
#define MAX_BUFFER_SIZE 5 //stellen im array
#define START_nothing_specified 0
#define END_nothing_specified 100

struct CircBuff{
    int write_index;
    int read_index;
    int terminate;
    int size;
    int client_amount;
    int client_tries;
    int buffer[MAX_BUFFER_SIZE];
};

void message(char *msg);
void error_exit(char *msg);
int generate_random(int start, int end);
void write_to_File(char *filename, int number, int tries, int client);
void aborted();


#endif //__COMMON_H__