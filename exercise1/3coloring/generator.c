#include "3col.h"



int main(int argc, char** argv){

    Matrix matrix;
    Solution solution;
    createMatrix(argc, argv, &matrix);

    removeAmount(&matrix, &solution);
    findEdges(&matrix, &solution);
    /*//DEBUGGING
    printMatrix(&matrix);
    printVertices(&matrix); 
    */

    
    //attach to shared memory
    int shmfd = shm_open(SHM_NAME, O_RDWR, 0666);
    if(shmfd == -1){//error
        fprintf(stderr, "Error: unable to open Shared Memory in %s.\n", argv[0]);
        return EXIT_FAILURE;
    }

    CircularBuffer *cb;
    cb = mmap(NULL, sizeof(CircularBuffer), PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);
    if(cb == MAP_FAILED){//error
        fprintf(stderr, "Error: cb unable to map in %s.\n", argv[0]);
        return EXIT_FAILURE;
    }
    if(close(shmfd) == -1){//error
        fprintf(stderr, "Error: cb unable to close in %s.\n", argv[0]);
        return EXIT_FAILURE;
    }

    //open semaphores
    sem_t *sfree = sem_open(SEM_FREE, 0);
    if(sfree == SEM_FAILED){
        fprintf(stderr, "Error: Semaphore FreeSpace in %s.\n", argv[0]);
        return EXIT_FAILURE;
    }

    sem_t *sused = sem_open(SEM_USED, 0);
    if(sused == SEM_FAILED){
        fprintf(stderr, "Error: Semaphore UsedSpace in %s.\n", argv[0]);
        return EXIT_FAILURE;
    }

    sem_t *smutex = sem_open(SEM_MUTEX, 0);
    if(smutex == SEM_FAILED){
        fprintf(stderr, "Error: Semaphore Mutex in %s.\n", argv[0]);
        return EXIT_FAILURE;
    }


    //initialize random seed
    srand(time(NULL)+clock()); //cast to remove compiler warning

    //loop until solution accepted
    while(!cb->flag){

        //generate random coloring
        for(int i = 0; i < matrix.vertices_count; i++){
            // 0 = rot
            // 1 = gelb
            // 2 = blau
            int r = rand() % 3;
            matrix.vertices[i] = r; //random farbe
        }
     
        //get amount of edges to remove
        removeAmount(&matrix, &solution);
        //skip if too many
        if(solution.edge_count > MAX_EDGES){
            continue;
        }
        findEdges(&matrix, &solution);

        /*//DEBUGGING
        printVertices(&matrix);
        printCurrentSolution(&solution);
        break;
        */

        //wait on free space semaphore
        if(sem_wait(sfree) == -1){
            if(errno == EINTR){
                continue; //interupted by signal?
            }
            fprintf(stderr, "Error: sem_wait FREE in %s.\n", argv[0]);
            return EXIT_FAILURE;
        }

        //wait on mutex

        if(sem_wait(smutex) == -1){
            fprintf(stderr, "Error: sem_wait MUTEX in %s.\n", argv[0]);
            return EXIT_FAILURE;
        }

        //check termination flag
        if(cb->flag){
            //release smutex and sfree
            sem_post(smutex);
            sem_post(sfree);
            break;
        }

        //write solution to buffer
        cb->data[cb->write_index] = solution;
        cb->write_index = (cb->write_index + 1) % cb->size;



#ifdef DEBUG
        fprintf(stderr, "Generator %d: Wrote solution with %d edges to remove.\n" , getpid() ,solution.edge_count);
#endif

        //release mutex
        if(sem_post(smutex) == -1){
            fprintf(stderr, "Error: sem_wait MUTEX in %s.\n", argv[0]);
            return EXIT_FAILURE;
        }

        //post to used 
        if(sem_post(sused) == -1){
            fprintf(stderr, "Error: sem_wait MUTEX in %s.\n", argv[0]);
            return EXIT_FAILURE;
        }

        //check flag again
        if(cb->flag){
            break;
        }

    }
    //terminate and clean up
    sem_close(sfree);
    sem_close(sused);
    sem_close(smutex);

    if(munmap(cb, sizeof(*cb)) == -1){//error
        fprintf(stderr, "Error: Shared memory unmap in %s.\n", argv[0]);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;

}//END main
