#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <errno.h>
#include <signal.h>
#include <time.h>

#define SHM_NAME "/vorbereitung_shm"
#define SEM_USED "/vorbereitungSemUsed"      //1. tracking used spac
#define SEM_FREE "/vorbereitungSemFree"      //2. tracking free space
#define SEM_MUTEX "/vorbereitungSemMutex"    //3. mutual exclusive
#define SHM_SIZE 50

#define errExit(msg)    do { perror(msg); exit(EXIT_FAILURE);} while (0)

#define MAX_LEN 256

struct myshm{
    int size;
    int read_index;
    int write_index;
    int terminate;
    char data[SHM_SIZE][MAX_LEN];
};

void wait_ms(int ms);




#endif