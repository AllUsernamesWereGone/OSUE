#include "common.h"




int main(int argc, char** argv){

    //user input einlesen
    int opt;
    int number = -1;
    if(argc != 3){
        error_exit("too many / less arguments\nUsage: ./generator [-c number != 0]\n");
    }
    while ((opt = getopt(argc, argv, "c:")) != -1) {
        switch (opt) {
        case 'c':
            if(number != -1){
                error_exit("Wrong arguments\n");
            }
            number = atoi(optarg);
            break;
        default: /* '?' */
            fprintf(stderr, "Usage: %s [-c number != 0]\n", argv[0]);
            error_exit("getopt default");
        }
    }

    //shm open
    int shmfd;
    shmfd = shm_open(SHM_NAME, O_RDWR, PERMISSIONS);
    if(shmfd == -1){
        error_exit("gen shm fd");
    }

    if(ftruncate(shmfd, sizeof(struct myshm)) == -1){
        error_exit("truncate shm gen");
    }

    struct myshm *shmp;
    shmp = mmap(NULL, sizeof(struct myshm), PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);
    if(shmp == MAP_FAILED){
        error_exit("map fail gen");
    }

    //semaphoren öffnen
    sem_t *sclient = sem_open(SEM_CLIENT, O_RDWR);
    if(sclient == SEM_FAILED){
        error_exit("sem client fail");
    }
    sem_t *sserver = sem_open(SEM_SERVER, O_RDWR);
    if(sserver == SEM_FAILED){
        error_exit("sem server fail");
    }
    if(number == 666){
        shmp->terminate = 1;
        if(sem_post(sserver) == -1){
            error_exit("client post");
        }
    }

    if(!shmp->terminate){
        if(sem_wait(sclient) == -1){
            error_exit("client wait");
        }
    
    
        //write to shm
        shmp->input = number;


    
    //semapohoren post for server
        if(sem_post(sserver) == -1){
            error_exit("client post");
        }

    }





    success("Success generator");
    return EXIT_SUCCESS;

}//END main