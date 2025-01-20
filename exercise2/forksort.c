#include "fork.h"




int main(int argc, char **argv) {

    unsigned int lineCount = 0;

    // Call the readInput function to read lines from stdin
    char **lines = readInput(stdin, &lineCount);

    // Check if the function succeeded
    if (!lines) {
        fprintf(stderr, "Failed to read input.\n");
        return EXIT_FAILURE;
    }

    //basecase
    if(lineCount == 1){ 
        fprintf(stdout, "%s\n", lines[0]);
        printf("only one line as input\n");
        free(lines); //free the whole array
        exit(EXIT_SUCCESS);
    }

    //split data 
    u_int leftCounter = lineCount/2;
    u_int rightCounter = lineCount - leftCounter;
    char **linesLeft = malloc(leftCounter * sizeof(char *));
    char **linesRight = malloc(rightCounter * sizeof(char *));

    if (!linesLeft || !linesRight) { //initialized properly?
        perror("malloc");
        free(lines);
        free(linesLeft);
        free(linesRight);
        exit(EXIT_FAILURE);
    }

    //copy data into new left and right arrays
    for(u_int i = 0; i < leftCounter; i++){
        linesLeft[i] = lines[i];
    }
    for(u_int i = 0; i < rightCounter; i++){
        linesRight[i] = lines[leftCounter + i];
    }

    //free original data, new arrays contain data
    free(lines);

    //pipe data
    int pipeLeft[2], pipeRight[2];
    if(pipe(pipeLeft) == -1 || pipe(pipeRight) == -1){ //create the pipes
        perror("error creating pipes");
        free(linesLeft);
        free(linesRight);
        exit(EXIT_FAILURE);
    }

    //left 
    pid_t leftChild = fork();

    if(leftChild == -1){
        fprintf(stderr, "error in forking left child");
        free(linesLeft);
        free(linesRight);
        exit(EXIT_FAILURE);
    }

    if(leftChild == 0){ //child left
        close(pipeLeft[0]);     //close read end of pipe, child doesnt need it
        close(pipeRight[0]);    //close read end of right pipe
        close(pipeRight[1]);    //close write end of right pipe




/* GPT
        dup2(pipeLeft[1], STDOUT_FILENO);   //redirect stdout to pipe
        
        //write data into pipes
        for (unsigned int i = 0; i < leftCounter; i++) {
            write(pipeLeft[1], linesLeft[i], strlen(linesLeft[i]) + 1);
        }
        close(pipeLeft[1]); // Close the write end after sending the data
*/
        FILE* writeStream = fdopen(pipeLeft[1], "w");
        if (!writeStream) {
            perror("fdopen");
            exit(EXIT_FAILURE);
        }

        for (unsigned int i = 0; i < leftCounter; i++) {
            fprintf(writeStream, "%s", linesLeft[i]);
        }
        fclose(writeStream);
        free(linesLeft);
        free(linesRight);




        //recursion
        execlp(argv[0], argv[0], NULL);

        //if execlp fails
        perror("execlp left failed");
        free(lines);
        free(linesLeft);
        free(linesRight);
        exit(EXIT_FAILURE);
    }

    //right
    pid_t rightChild = fork();

    if(rightChild == -1){
        fprintf(stderr, "error in forking left child");
        free(lines);
        free(linesLeft);
        free(linesRight);
        exit(EXIT_FAILURE);
    }

    if(rightChild == 0){ //child right
        close(pipeRight[0]);    //close read end of pipe, child doesnt need it
        close(pipeLeft[0]);     //close read end of left pipe
        close(pipeLeft[1]);     //close write end of left pipe




/* GPT
        dup2(pipeRight[1], STDOUT_FILENO);   //redirect stdout to pipe

        //write data into pipes
        for (unsigned int i = 0; i < rightCounter; i++) {
            write(pipeRight[1], linesRight[i], strlen(linesRight[i]) + 1);
        }

        close(pipeRight[1]); // Close the write end after sending the data
*/
        FILE* writeStream = fdopen(pipeRight[1], "w");
        if (!writeStream) {
            perror("fdopen");
            exit(EXIT_FAILURE);
        }

        for (unsigned int i = 0; i < rightCounter; i++) {
            fprintf(writeStream, "%s", linesRight[i]);
        }
        fclose(writeStream);
        free(linesLeft);
        free(linesRight);




        //recursion
        execlp(argv[0], argv[0], NULL);

        //if execlp fails
        perror("execlp left failed");
        free(lines);
        free(linesLeft);
        free(linesRight);
        exit(EXIT_FAILURE);
    }

    //parent processes from here on
  
    close(pipeLeft[1]);        
    close(pipeRight[1]);  


    FILE *leftStream = fdopen(pipeLeft[0], "r");
    FILE *rightStream = fdopen(pipeRight[0], "r");

    if (!leftStream || !rightStream){
        perror("Error opening pipes");
        free(lines);
        free(linesLeft);
        free(linesRight);
        exit(EXIT_FAILURE);
    }

    //rest of the code
        
    //TODO--------------------------------------------------------------


    //GPT
    char buffer[1024];
    char** sortedLeft = malloc(leftCounter * sizeof(char*));
    char** sortedRight = malloc(rightCounter * sizeof(char*));
    unsigned int leftIndex = 0, rightIndex = 0;

    while (fgets(buffer, sizeof(buffer), leftStream)) {
        sortedLeft[leftIndex++] = strdup(buffer);
    }
    while (fgets(buffer, sizeof(buffer), rightStream)) {
        sortedRight[rightIndex++] = strdup(buffer);
    }

    fclose(leftStream);
    fclose(rightStream);







    // Wait for both children to finish
    int status;
    waitpid(leftChild, &status, 0);
    if (WEXITSTATUS(status) == EXIT_FAILURE) {
        fprintf(stderr, "error in left child process.. \n");
        exit(EXIT_FAILURE);
    }
    waitpid(rightChild, &status, 0);
    if (WEXITSTATUS(status) == EXIT_FAILURE) {
        fprintf(stderr, "error in right child process.. \n");
        exit(EXIT_FAILURE);
    }


    //test ----------------------------
    /*
    //char **lines = readInput(stdin, &lineCount);
    char **sortedLeft = readInput(leftStream, &leftCounter);
    char **sortedRight = readInput(rightStream, &rightCounter);
    fclose(leftStream);
    fclose(rightStream);
*/


    //merge
    merge(stdout, sortedLeft, sortedRight, leftCounter, rightCounter);

    //Free all memory

    close(pipeLeft[0]);    
    close(pipeLeft[1]);    
    close(pipeRight[0]);    
    close(pipeRight[1]);  

    for (unsigned int i = 0; i < leftCounter; i++) {
        free(sortedLeft[i]);
    }
    free(sortedLeft);

    for (unsigned int i = 0; i < rightCounter; i++) {
        free(sortedRight[i]);
    }
    free(sortedRight);


    //free the rest
    for (unsigned int i = 0; i < leftCounter; i++) {
        free(linesLeft[i]);
    }
    free(linesLeft);

    for (unsigned int i = 0; i < rightCounter; i++) {
        free(linesRight[i]);
    }
    free(linesRight);


    return EXIT_SUCCESS;;
}