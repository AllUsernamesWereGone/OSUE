#include "common.h"

volatile sig_atomic_t quit = 0;

void handle_signal(int signal){
    quit = 1;
}


int main(int argc, char** argv){

    //setup signal handling
    struct sigaction sa = { .sa_handler = handle_signal };
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);


    if(argc < 5 || argc > 6){
        error_exit("Usage: client [-k terminate] -s range start, -e range end\n");
    }
    int opt;
    int terminate = 0;
    int range_start = 0;
    int range_end = 0;


    
    while ((opt = getopt(argc, argv, "ks:e:")) != -1) {
        switch (opt) {
        case 'k': //terminate all generators
            terminate = 1;
            break;
        case 's':
            range_start = atoi(optarg);
            break;
        case 'e':
            range_end = atoi(optarg);
            break;
        default: /* '?' */
            error_exit("Usage: client [-k terminate] -s range start, -e range end\n");
        }
    }

    //printf("terminate=%d; range_start=%d; range_end=%d; optind=%d\n",
    //        terminate,    range_start,    range_end,    optind);

    if(range_start >= range_end || range_start < 0 || range_end < 0){
        error_exit("Ranges wrong\n");
    }

    //open shared memory
    int shmfd = shm_open(SHM_NAME, O_RDWR, PERMISSIONS);
    if(shmfd == -1){
        error_exit("client shm open");
    }
    
    /* nicht nötig
    if(ftruncate(shmfd, sizeof(struct CircBuff)) == -1){
        error_exit("client ftruncate");
    }
    */

    struct CircBuff *cb;
    cb = mmap(NULL, sizeof(*cb), PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);
    if(cb == MAP_FAILED){
        error_exit("client mmap");
    }
    
    //open semaphores
    sem_t *Sfree = sem_open(SEM_FREE, O_RDWR);
    if(Sfree == SEM_FAILED){
        error_exit("client Sfree");
    }
    sem_t *Sused = sem_open(SEM_USED, O_RDWR);
    if(Sused == SEM_FAILED){
        error_exit("client Sused");
    }
    sem_t *Smutex = sem_open(SEM_MUTEX, O_RDWR);
    if(Smutex == SEM_FAILED){
        error_exit("client Smutex");
    }



    if(terminate){
        cb->terminate = terminate;
    }

    //add number to clients to ensure safe exit
    cb->client_amount += 1;

    //while schleife
    while(!quit && !cb->terminate){

        if(sem_wait(Sfree) == -1){
            if(errno == EINTR){
                if(quit){
                    cb->terminate = 1;
                }
                continue;
            }
            error_exit("wait client");
        }

        if(sem_wait(Smutex) == -1){
            if(errno == EINTR){
                if(quit){
                    cb->terminate = 1;
                }
                continue;
            }
            error_exit("wait mutex");
        }

        if(cb->terminate){
            sem_post(Smutex);
            sem_post(Sused);
            break;
        }

        
        //generate numbers and write to shm
        cb->client_tries += 1;
        cb->buffer[cb->write_index] = generate_random(range_start, range_end);
        cb->write_index = ((cb->write_index + 1) % cb->size);

        if(sem_post(Smutex) == -1){
            error_exit("post mutex");
        }
        if(sem_post(Sused) == -1){
            error_exit("post used");
        }

    }
    //end schleife


    //close and cleanup needed?
    /*
    sem_close(cb->Sfree);
    sem_close(cb->Smutex);
    sem_close(cb->Sused);

    sem_unlink(SEM_USED);
    sem_unlink(SEM_FREE);
    sem_unlink(SEM_MUTEX);

    shm_unlink(SHM_NAME);
    */

    message("Client finished");
    fflush(stdout);
    return EXIT_SUCCESS;
}
