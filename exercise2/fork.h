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
void merge(FILE* output, char** arr1, char** arr2, u_int counter1, u_int counter2);


#endif // FORK_H