#include "common.h"

void message(char *msg){
    printf("%s\n", msg);
}
void error_exit(char *msg){
    printf("Error: %s\n", msg);
    exit(EXIT_FAILURE);
}
int generate_random(int min, int max){
    srand(time(NULL)+clock());
    int num =  (rand() %  (max - min + 1)  + min);
    //printf("generate random = %i\n" , num);
    return num;
}

void write_to_File(char *filename, int number, int tries, int client){
    if(strcmp(filename, "stdout") == 0){
        printf("Number was found: %i\nTries to find: %i\nClient tries: %i\n", number, tries, client);
    }else{

        // Print to a file
        FILE *fp = fopen(filename, "w"); //opens file descriptor and sets permission
        if (fp == NULL) {
            perror("fopen");
            return;
        }

        fprintf(fp, "Number was found: %i\nTries to find: %i\nClient tries: %i\n", number, tries, client); //writes to file
        fclose(fp); //closes file descriptor

    }

}
void aborted(){
    printf("Clients closed, Server shutting down.\n");
}



