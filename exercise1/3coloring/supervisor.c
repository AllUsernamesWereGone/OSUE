#include "3col.h"


volatile sig_atomic_t quit = 0;

void handle_signal(int signal){
    quit = 1;
}

int main(int argc, char** argv){

    //setup signal handling
    struct sigaction sa = { .sa_handler = handle_signal };
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);


    //get input for supervisor
    int opt;
    int limit = -1; //if not specified, infinite
    int delay = 0;  //if not specified, zero seconds

    while((opt = getopt(argc, argv, "n:w:"))!=-1){
        switch(opt){
            case 'n':
                limit = atoi(optarg);
                break;
            case 'w':
                delay = atoi(optarg);
                break;
            default:
                fprintf(stderr, "Error: Unrecognized option in %s.\n", argv[0]);
                return EXIT_FAILURE;
        }
    }
    

    //generators, initialize shared memory etc
    
    //initialize shared memory
    int shmfd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666); //0-rw-rw-rw jeder darf read / write
    if(shmfd == -1){//error
        fprintf(stderr, "Error: Shared Memory in %s.\n", argv[0]);
        return EXIT_FAILURE;
    }

    //set size of shared memory
    if(ftruncate(shmfd, sizeof(CircularBuffer)) == -1){
        fprintf(stderr, "Error: Shared Memory Size in %s.\n", argv[0]); 
        return EXIT_FAILURE;
    }

    //map shared memory
    CircularBuffer *cb;
    cb = mmap(NULL, sizeof(CircularBuffer), PROT_READ | PROT_WRITE, MAP_SHARED, shmfd,0);
    if(cb == MAP_FAILED){//error
        fprintf(stderr, "Error: Shared Memory mapping in %s.\n", argv[0]); 
        return EXIT_FAILURE;
    }
    if(close(shmfd) == -1){//error
        fprintf(stderr, "Error: Shared Memory closing in %s.\n", argv[0]); 
        return EXIT_FAILURE;
    }

    //initialisiere CircularBuffer
    cb->read_index = 0;
    cb->write_index = 0;
    cb->size = BUFFER_SIZE;
    cb->flag = 0;


    //initialze / open semaphores
    
    //SEM_FREE free space tracker
    sem_t *sfree = sem_open(SEM_FREE, O_CREAT, 0666, BUFFER_SIZE);//BUFFER_SIZE = alle frei
    if(sfree == SEM_FAILED){
        fprintf(stderr, "Error: Semaphore FreeSpace in %s.\n", argv[0]); 
        return EXIT_FAILURE;
    }

    //SEM_USED used space tracker
    sem_t *sused = sem_open(SEM_USED, O_CREAT, 0666, 0);//0 = nichts verwendet
    if(sused == SEM_FAILED){
        fprintf(stderr, "Error: Semaphore UsedSpace in %s.\n", argv[0]); 
        return EXIT_FAILURE;
    }
    
    //SEM_MUTEX mutual exclusion
    sem_t *smutex = sem_open(SEM_MUTEX, O_CREAT, 0666, 1);// 1 = noch kein prozess
    if(smutex == SEM_FAILED){
        fprintf(stderr, "Error: Semaphore Mutex in %s.\n", argv[0]); 
        return EXIT_FAILURE;
    }
    
    //read solutions after initial delay
    sleep(delay);


    //variables for loop
    int solutions_read = 0;
    int best_edge_count = -1; //nothing found yet
    Solution best_solution;




    //loop until solution found
    while(!quit && (limit == -1 || solutions_read < limit)){

        //wait on used semaphore
        if(sem_wait(sused) == -1){//error
            if(errno == EINTR){
                continue; //interrupted by signal?
             }
            fprintf(stderr, "Error: sem_wait USED in %s.\n", argv[0]);
            return EXIT_FAILURE;
        }            

        //Read solution from buffer
        
        Solution solution = cb->data[cb->read_index];
        cb->read_index = (cb->read_index + 1) % cb->size;


        //post on free semaphoe
        if(sem_post(sfree) == -1){
            fprintf(stderr, "Error: sem_post FREE in %s.\n", argv[0]);
        }

        solutions_read++;


        //check if is best solution
        //TODO
        if(best_edge_count == -1 || solution.edge_count < best_edge_count){
            best_edge_count = solution.edge_count;
            best_solution = solution;
        }

        //if solution with 0 edges found
        if(best_edge_count == 0){
            printf("The graph is 3-colorable!\n");
            cb->flag = 1;
            break;
        }


    }
    if(best_edge_count > 0){
        printCurrentSolution(&best_solution);
    }

    //terminate all generators, unlink shared resources

    sem_post(sfree);// TODO wtf, musste hinzugefügt werden damit generator abbricht -------------------
    //notify generators to terminate
    cb->flag = 1;

    sem_close(sfree);
    sem_close(sused);
    sem_close(smutex);

    sem_unlink(SEM_FREE);
    sem_unlink(SEM_USED);
    sem_unlink(SEM_MUTEX);

    
    if(munmap(cb, sizeof(*cb)) == -1){//error
        fprintf(stderr, "Error: Shared Memory unmapping in %s.\n",  argv[0]);
        return EXIT_FAILURE;
    }
    
    if(shm_unlink(SHM_NAME) == -1){
        fprintf(stderr, "Error: Shared Memory unlinking in %s.\n",  argv[0]);
        return EXIT_FAILURE;
    }
    
    printf("Solutions read: %d\n", solutions_read);


    return EXIT_SUCCESS;
}//END main
