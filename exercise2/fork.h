#ifndef FORK_H
#define FORK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>



char** readInput(FILE *dataInput,unsigned int *count);


#endif // FORK_H