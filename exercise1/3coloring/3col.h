#ifndef __3COL_H__
#define __3COL_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <fcntl.h> //for O_ constants
#include <sys/mman.h> //shared memory
#include <semaphore.h>
#include <errno.h> //errno function, error handling
#include <signal.h>

#define MAX_EDGES (8) //hinweis in angabe
#define MAX_VERTICES (35) //angabe hat max 33
#define BUFFER_SIZE (4096) //teste teste teste -----------

#define SHM_NAME "/3col_shm"
#define SEM_USED "/3colSemUsed"      //1. tracking used spac
#define SEM_FREE "/3colSemFree"      //2. tracking free space
#define SEM_MUTEX "/3colSemMutex"    //3. mutual exclusive

//algemein

//Adjazenz-Matrix
typedef struct{
    int vertices[MAX_VERTICES]; 
    int vertices_count;
    int edges;
    int admatrix[MAX_VERTICES][MAX_VERTICES]; 
}Matrix;

typedef struct{
    int u;
    int v;
}Edge;

typedef struct{
    int edge_count;
    Edge edges[MAX_EDGES];
}Solution;

//Circular Buffer
typedef struct{
    int read_index;
    int write_index;
    int flag;
    int size;
    Solution data[BUFFER_SIZE];
}CircularBuffer;


void createMatrix(int argc, char** argv, Matrix *matrix);
void printMatrix(Matrix *matrix);
void printVertices(Matrix *matrix);
void removeAmount(Matrix *matrix, Solution *solution);
void findEdges(Matrix *matrix, Solution *solution);
void printCurrentSolution(Solution *solution);

#endif //__3COL_H__

