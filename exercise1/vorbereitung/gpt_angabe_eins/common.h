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


#define SHM_NAME "/gpt_shm"
#define SEM_CLIENT "/sem_client"
#define SEM_SERVER "/sem_server"
#define PERMISSIONS 0666

struct myshm{
    int input;
    int result;
    int terminate;
};


void success(char *msg);
void error_exit(char *msg);
int calc_input(int number);

/*
server starten
./supervisor
client liest mittels getopt zahlen ein
./generator -c number

nach einlesen gibt client die zahl aus
nach verarbeiten gibt server eine meldung aus und wartet auf neues
falls die zahl 666 ist beendet der server alle clients und sich

*/







#endif //__COMMON_H__