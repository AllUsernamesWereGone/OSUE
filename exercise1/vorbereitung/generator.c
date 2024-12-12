#include "common.h"

void failure(char* argv[]){
    printf("ERROR -------------\n");
    fprintf(stderr, "Usage: %s [-r repeat x times] [-d 50ms delay] [-n name] name \n",argv[0]);
    exit(EXIT_FAILURE); 
}

volatile sig_atomic_t quit = 0;
void handle_signal(int signal) { quit = 1; }


int main(int argc, char** argv){

    struct sigaction sa = { .sa_handler = handle_signal };
    sigaction(SIGINT, &sa, NULL);

    if(argc <= 1 || argc >6){
        fprintf(stderr, "AAUsage: %s [-r repeat x times] [-d 50ms delay] name \n",argv[0]);
        exit(EXIT_FAILURE); 
    }

    int repeat;
    int delay;
    char* name;
    int ms = 100;//anzahl millisekunden zum warten   1000 = 1 sek

    repeat = -1;
    delay = -1;
    name = NULL;
    
    int opt;
    while ((opt = getopt(argc, argv, "r:d")) != -1) {
        switch (opt) {
        case 'r':
            if(repeat != -1){
                failure(&argv[0]);
            }
            repeat = atoi(optarg);
            break;
        case 'd':
            if(delay != -1){
                failure(&argv[0]);
            }
            delay = ms; 
            break;
        default: /* '?' */
            failure(&argv[0]);
        }
    }

    printf("repeat=%d; delay=%d; name=%s; optind=%d\n",repeat, delay, name, optind);

    if (optind >= argc ) {
        fprintf(stderr, "--------incorrect usage----------\n");
        exit(EXIT_FAILURE);
    }
    name = argv[optind];

    printf("repeat=%d; delay=%d; name=%s; optind=%d\n",repeat, delay, name, optind);
    printf("name argument = %s\n", argv[optind]);


    //shared memory init

    /* Open the existing shared memory object and map it
    into the caller's address space */

    int shmfd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (shmfd == -1)
        errExit("shm_open");

    
    if (ftruncate(shmfd, sizeof(struct myshm)) == -1){
        errExit("ftruncate");
    }

    struct myshm *myshm; 
    myshm = mmap(NULL, sizeof(struct myshm),PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);
    if (myshm == MAP_FAILED)
        errExit("mmap");

    if(close(shmfd) == -1){//error
        errExit("mmap");
    }


    //semaphoren init
            sem_t *s1 = sem_open(SEM_USED, O_RDWR);
               if(s1== SEM_FAILED)
                   errExit("SEM_USED");
               sem_t *s2  = sem_open(SEM_FREE, O_RDWR);
               if(s2== SEM_FAILED)
                   errExit("SEM_FREE");
                sem_t *s3 = sem_open(SEM_MUTEX, O_RDWR);
                if(s3== SEM_FAILED)
                   errExit("SEM_MUTEX");



    if(repeat >= 1){
        for(int i = 0; i < repeat; i++){

            //semaphoren
            if(sem_wait(s2) == -1){
                if(errno == EINTR)
                    continue;
                errExit("sem wait");
            }

            //write to shared memory
            printf("Index %d: ", myshm->write_index);

            strncpy(myshm->data[myshm->write_index], name, MAX_LEN);
            myshm->data[myshm->write_index][MAX_LEN - 1] = '\0';

            //myshm->data[myshm->write_index] = name;
            printf("%s \n", myshm->data[myshm->write_index]);
            //increment write index
            myshm->write_index = (myshm->write_index +1) % myshm->size;
                
            //semaphoren
            if(sem_post(s1) == -1){
                errExit("sem post");
            }

            //delay
            if(delay != -1){
                wait_ms(delay);
            }
        }

    
    }
    //close semaphoren
    sem_close(s1);
    sem_close(s2);
    sem_close(s3);

    //close shared memory
    if(munmap(myshm, sizeof(*myshm)) == -1){//error
        errExit("unmap shm");
    }

    
    //free resources

    exit(EXIT_SUCCESS);  

}