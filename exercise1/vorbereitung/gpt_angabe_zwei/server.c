#include "common.h"

volatile sig_atomic_t quit = 0;

void handle_signal(int signal){
    quit = 1;
}

void cleanup(){
    //delete and unlink everything before
    printf("cleanup....   ");
    shm_unlink(SHM_NAME);
    sem_unlink(SEM_FREE);
    sem_unlink(SEM_USED);
    sem_unlink(SEM_MUTEX);

    printf("cleanup finished\n");
    fflush(stdout);
}


int main(int argc, char** argv){
    
    cleanup();

    //setup signal handling
    struct sigaction sa = { .sa_handler = handle_signal };
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);


    int opt;
    int number = -1;
    char *output_file = "stdout";

    if(argc > 6){
        error_exit("Usage: server [-f file name] [-n number]\n");
    }

    
    while ((opt = getopt(argc, argv, "f:n:")) != -1) {
        switch (opt) {
        case 'f':
            output_file = optarg;
            break;
        case 'n':
            number = atoi(optarg);
            if(number < 0){
                error_exit("Number cant be negative.\n");
            }
            break;
        default: /* '?' */
            error_exit("Usage: server [-f file name] [-n number]\n");
        }
    }

    if(number == -1){
        number = generate_random(START_nothing_specified, END_nothing_specified);
    }

    //printf("output file=%s; number=%d;  optind=%d\n",
    //        output_file, number, optind);

    int shmfd = shm_open(SHM_NAME, O_CREAT | O_RDWR, PERMISSIONS);
    if(shmfd == -1){
        error_exit("server shm open");
    }

    if(ftruncate(shmfd, sizeof(struct CircBuff)) == -1){
        error_exit("server ftruncate");
    }

    struct CircBuff *cb;
    cb = mmap(NULL, sizeof(*cb), PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);
    if(cb == MAP_FAILED){
        error_exit("server mapping");
    }
    
    
    sem_t *Sfree = sem_open(SEM_FREE, O_CREAT | O_RDWR, PERMISSIONS, MAX_BUFFER_SIZE);
    if(Sfree == SEM_FAILED){
        error_exit("server Sfree");
    }
    sem_t *Sused = sem_open(SEM_USED, O_CREAT | O_RDWR, PERMISSIONS, 0);
    if(Sused == SEM_FAILED){
        error_exit("server Sused");
    }
    sem_t *Smutex = sem_open(SEM_MUTEX, O_CREAT | O_RDWR, PERMISSIONS, 1);
    if(Smutex == SEM_FAILED){
        error_exit("server Smutex");
    }

    //cb initialize
    cb->size = MAX_BUFFER_SIZE;
    cb->write_index = 0;
    cb->read_index = 0;
    cb->terminate = 0;
    cb->client_amount = 0;
    cb->client_tries = 0;
    int tries = 0;


    while(!quit && !cb->terminate){

        if(sem_wait(Sused) == -1){
            if(errno == EINTR){
                if(quit){
                    cb->terminate = 1;
                }
                continue;
            }
            error_exit("server sem wait");
        }

        //read from buffer and compare results

        tries++;
        if(cb->buffer[cb->read_index] == number){
            //correct
            write_to_File(output_file, number, tries, cb->client_tries);
            cb->terminate = 1;
        }

        cb->read_index = ((cb->read_index + 1) % cb->size);

        if(sem_post(Sfree) == -1){
            error_exit("server sem post");
        }

    }

    for(int i = 0; i < cb->client_amount; i++){
        sem_post(Sfree);
        sem_post(Smutex);
    }

    if(cb->terminate){
        aborted();
    }
    if(quit){
        error_exit("Signal interrupt.");
    }
    

    //cleanup
    sem_close(Sfree);
    sem_close(Smutex);
    sem_close(Sused);

    sem_unlink(SEM_USED);
    sem_unlink(SEM_FREE);
    sem_unlink(SEM_MUTEX);

    shm_unlink(SHM_NAME);


    message("Server finished");
    return EXIT_SUCCESS;
}