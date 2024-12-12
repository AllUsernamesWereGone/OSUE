#include "common.h"

volatile sig_atomic_t quit = 0;
void handle_signal(int signal) { quit = 1; }


int main(int argc, char** argv){


    struct sigaction sa = { .sa_handler = handle_signal };
    sigaction(SIGINT, &sa, NULL);





    int counter;
    if(argc == 2){
        counter = atoi(argv[1]);
    }else{
        counter = 3;
    }

    //shm unlinken 
    if(shm_unlink(SHM_NAME)==-1){
        printf("no SHM to unlink\n");
    }
    //sem unlinken
    if(sem_unlink(SEM_USED)==-1){
        printf("no SEM_USED to unlink\n");
    }
    if(sem_unlink(SEM_FREE)==-1){
        printf("no SEM_FREE to unlink\n");
    }
    if(sem_unlink(SEM_MUTEX)==-1){
        printf("no SEM_MUTEX to unlink\n");
    }

    printf("made it past unlink\n");

    //init & open shared memory

    
    int shmfd;
    shmfd = shm_open(SHM_NAME, O_RDWR | O_CREAT, 0666);

    if (shmfd == -1){
        errExit("shm_open");
    }

    
    if (ftruncate(shmfd, sizeof(struct myshm)) == -1){
        errExit("ftruncate");
    }

    /* Map the object into the caller's address space. */
    struct myshm *myshm;
    myshm = mmap(NULL, sizeof(struct myshm), PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);
    if (myshm == MAP_FAILED){
        errExit("mmap");
    }

    myshm->size = SHM_SIZE;
    myshm->read_index = 0;
    myshm->write_index = 0;
    myshm->terminate = 0;
            
    if(close(shmfd) == -1){
        errExit("close shm");
    }

    //init semaphoren
    sem_t *s1 = sem_open(SEM_USED, O_CREAT | O_EXCL,0666, 0);
    if(s1 == SEM_FAILED){
        errExit("SEM_USED");
    }

    sem_t *s2  = sem_open(SEM_FREE, O_CREAT | O_EXCL,0660, SHM_SIZE);
    if(s2 == SEM_FAILED){
        errExit("SEM_FREE");
    }
    sem_t *s3 = sem_open(SEM_MUTEX, O_CREAT | O_EXCL,0660, 1);
    if(s3 == SEM_FAILED){
        errExit("SEM_MUTEX");
    }

    sleep(5);
    char* input;
    while(!quit && counter >0){


        //semaphoren
        if(sem_wait(s1) == -1){
            if(errno == EINTR)
                continue;
            errExit("sem wait");
        }

        //read from shared memory

        input = myshm->data[myshm->read_index];

        //input = myshm->data[myshm->read_index];
        printf("%s\n", input);
        //increment read_index
        myshm->read_index = (myshm->read_index +1) % myshm->size;


        //semaphoren
        if(sem_post(s2) == -1){
            errExit("sem wait");
        }

        sleep(1);
        
        counter--;
    }
    printf("nach schleife\n");

    //close shared memory
    

    //close semaphoren 
    if(sem_close(s1) == -1){
        errExit("sem free");
    }
    if(sem_close(s2) == -1){
        errExit("sem used");
    }
    if(sem_close(s3) == -1){
        errExit("sem mutex");
    }

    //free memory if needed
    if(sem_unlink(SEM_MUTEX) == -1){
        errExit("unlink mutex");
    }
    if(sem_unlink(SEM_USED) == -1){
        errExit("unlink used");
    }
    if(sem_unlink(SEM_FREE) == -1){
        errExit("unlink free");
    }


    if(munmap(myshm, sizeof(*myshm)) == -1){
        errExit("shm unmap");
    }
    if(shm_unlink(SHM_NAME) == -1){
        errExit("shm unlink");
    }



    printf("SUCCESS\n");
    exit(EXIT_SUCCESS);


}