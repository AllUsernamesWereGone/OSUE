#include "common.h"


volatile sig_atomic_t quit = 0;

void handle_signal(int signal){
    quit = 1;
}

void cleanup(){
    //delete and unlink everything before
    printf("cleanup\n");
    shm_unlink(SHM_NAME);
    sem_unlink(SEM_CLIENT);
    sem_unlink(SEM_SERVER);

    printf("cleanup finished\n");
    fflush(stdout);
}

int main(int argc, char** argv){

    cleanup();

    //setup signal handling
    struct sigaction sa = { .sa_handler = handle_signal };
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);


    //shared memory initialisieren
    int shmfd = shm_open(SHM_NAME, O_CREAT | O_EXCL | O_RDWR, PERMISSIONS);
    if(shmfd == -1){
        if(errno == EEXIST){
            error_exit("shm open and cleanup fail");
        }
        error_exit("shm open");
    }

    if(ftruncate(shmfd, sizeof(struct myshm)) == -1){
        error_exit("truncate shm");
    }

    struct myshm *shmp;

    shmp = mmap(NULL, sizeof(struct myshm), PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);
    if(shmp == MAP_FAILED){
        error_exit("mapping shm");
    }

    //semaphoren initialisieren
    sem_t *sclient = sem_open(SEM_CLIENT, O_CREAT | O_RDWR, PERMISSIONS, 1);
    if(sclient == SEM_FAILED){
        error_exit("sem client fail");
    }

    sem_t *sserver = sem_open(SEM_SERVER, O_CREAT | O_RDWR, PERMISSIONS, 0);
    if(sserver == SEM_FAILED){
        error_exit("sem server fail");
    }

    
    //flag zum terminieren
    shmp->terminate = 0;
    

    //while schleife zum verarbeiten
    while(!shmp->terminate){
        if(quit){
            shmp->terminate = 1;
        }
        
        if(sem_wait(sserver) == -1){
            if(errno == EINTR){
                if(quit){
                    shmp->terminate = 1;
                }
                continue;
            }
            error_exit("server wait");
        }
        if(shmp->terminate){
            break;
        }
        sleep(1);
        //read from shm and write
        shmp->result = calc_input(shmp->input);
        printf("The input was: %i\nThe Result is (*2): %i\n", shmp->input, shmp->result);
        shmp->input = 0;
        shmp->result = 0;

        if(sem_post(sclient) == -1){
            error_exit("client post");
        }

        if(quit){
            shmp->terminate = 1;
        }

    }

    //alles schließen und unlinken

    sem_close(sclient);
    sem_close(sserver);
    sem_unlink(SEM_CLIENT);
    sem_unlink(SEM_SERVER);

    shm_unlink(SHM_NAME);



    success("Success supervisor");
    return EXIT_SUCCESS;

}//END main