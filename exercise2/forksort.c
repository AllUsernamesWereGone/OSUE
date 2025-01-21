#include "fork.h"




int main(int argc, char **argv) {

    unsigned int lineCount = 0;

/*
    if (argc > 1 && strcmp(argv[1], "left") == 0) {
        // Read from stdin (pipeLeft)
        char** lines = readInput(stdin, &lineCount);
        if (lineCount == 1) {
            fprintf(stdout, "%s", lines[0]);
            free(lines);
            return EXIT_SUCCESS;
        }
    } else if (argc > 1 && strcmp(argv[1], "right") == 0) {
        // Read from stdin (pipeRight)
        char** lines = readInput(stdin, &lineCount);
        if (lineCount == 1) {
            fprintf(stdout, "%s", lines[0]);
            free(lines);
            return EXIT_SUCCESS;
        }
    }
*/
    printf("linecount  : %d\n", lineCount);

    // Call the readInput function to read lines from stdin
    char **lines = readInput(stdin, &lineCount);
    // Check if the function succeeded
    if (!lines) {
        fprintf(stderr, "Failed to read input.\n");
        return EXIT_FAILURE;
    }

    printf("linecount 2: %d\n", lineCount);

    fflush(stdout);

    
    if(lineCount <= 1){ 
        //fprintf(stdout, "%s", lines[0]);  //segfault wegen lines[0]
        printf("only one line as input\n");
        free(lines); //free the whole array
        exit(EXIT_SUCCESS);
    }

    printf("linecount 3: %d\n", lineCount);

    //split data 
    u_int leftCounter = lineCount / 2;
    u_int rightCounter = lineCount - leftCounter;
    char **linesLeft = malloc(leftCounter * sizeof(char *));
    char **linesRight = malloc(rightCounter * sizeof(char *));


    if (!linesLeft || !linesRight) { //initialized properly?
        perror("malloc\n");
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
        perror("error creating pipes\n");
        free(linesLeft);
        free(linesRight);
        exit(EXIT_FAILURE);
    }

    //left 
    pid_t leftChild = fork();

    if(leftChild == -1){
        fprintf(stderr, "error in forking left child\n");
        free(linesLeft);
        free(linesRight);
        exit(EXIT_FAILURE);
    }

    if(leftChild == 0){ //child left
        close(pipeRight[0]);    //close read end of right pipe
        close(pipeRight[1]);    //close write end of right pipe

        //dup2(pipeLeft[0], STDIN_FILENO);
        dup2(pipeLeft[1], STDOUT_FILENO);   //redirect stdout to pipe

        
        //recursion
        execlp(argv[0], argv[0], NULL);
        //execlp(argv[0], argv[0], "left", NULL);

        //if execlp fails
        perror("execlp left failed\n");
        free(lines);
        free(linesLeft);
        free(linesRight);
        exit(EXIT_FAILURE);
    }

    //right
    pid_t rightChild = fork();

    if(rightChild == -1){
        fprintf(stderr, "error in forking left child\n");
        free(lines);
        free(linesLeft);
        free(linesRight);
        exit(EXIT_FAILURE);
    }

    if(rightChild == 0){ //child right
        close(pipeLeft[0]);     //close read end of left pipe
        close(pipeLeft[1]);     //close write end of left pipe

        //dup2(pipeRight[0], STDIN_FILENO);
        dup2(pipeRight[1], STDOUT_FILENO);   //redirect stdout to pipe

        close(pipeRight[0]);    //close read end of right pipe
        close(pipeRight[1]);

        //recursion
        execlp(argv[0], argv[0], NULL);
        //execlp(argv[0], argv[0], "right", NULL);

        //if execlp fails
        perror("execlp left failed\n");
        free(lines);
        free(linesLeft);
        free(linesRight);
        exit(EXIT_FAILURE);
    }

    //parent processes from here on

    //close(pipeLeft[0]);        
    //close(pipeRight[0]); 
    //close(pipeLeft[1]);        
    //close(pipeRight[1]);  


    FILE *leftIn = fdopen(pipeLeft[1], "w");
    if (leftIn == NULL) {
        perror("Error opening pipe for writing (left)\n");
        free(linesLeft);
        free(linesRight);
        exit(EXIT_FAILURE);
    }
   
    FILE *rightIn = fdopen(pipeRight[1], "w");
    if (rightIn == NULL) {
        perror("Error opening pipe for writing (right)\n");
        free(linesLeft);
        free(linesRight);
        exit(EXIT_FAILURE);
    }

    for(int i = 0; i < leftCounter; i++){
        fprintf(leftIn, "%s\n", linesLeft[i]);
    }

    for(int i = 0; i < rightCounter; i++){
        fprintf(rightIn, "%s\n", linesRight[i]);
    }

    free(linesLeft);
    free(linesRight);
    fclose(leftIn);
    fclose(rightIn);

    close(pipeLeft[1]);        
    close(pipeRight[1]); 




    
    //rest of the code
        
    //TODO--------------------------------------------------------------
    // Wait for both children to finish
    int status;

    printf("before wait\n");
    fflush(stdout);

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

    printf("after wait\n");
    fflush(stdout);


    FILE *leftStream = fdopen(pipeLeft[0], "r");
    FILE *rightStream = fdopen(pipeRight[0], "r");

    if (!leftStream || !rightStream){
        perror("Error opening pipes\n");
        free(lines);
        free(linesLeft);
        free(linesRight);
        exit(EXIT_FAILURE);
    }


    //GPT
    //segfault----------------------------------------------------------------------------
    /*
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
    */
    


    

    //test ----------------------------
    
    //char **lines = readInput(stdin, &lineCount);
    char **sortedLeft = readInput(leftStream, &leftCounter);
    char **sortedRight = readInput(rightStream, &rightCounter);
    fclose(leftStream);
    fclose(rightStream);


    close(pipeLeft[0]);        
    close(pipeRight[0]); 


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