#include "fork.h"




int main(int argc, char **argv) {
    

    
    mydata_t data;

    // Call the readInput function to read lines from stdin
    data.lines = readInput(stdin, &data.Count);
    // Check if the function succeeded
    if (!data.lines) {
        fprintf(stderr, "Failed to read input.\n");
        return EXIT_FAILURE;
    }

    printf("linecount 2: %d\n", data.Count);

    fflush(stdout);

    
    if(data.Count == 1){ 
        fprintf(stdout, "%s", data.lines[0]);  //segfault wegen lines[0]
        printf("only one line as input\n");
        free(data.lines); //free the whole array
        exit(EXIT_SUCCESS);
    }
    

    printf("linecount 3: %d\n", data.Count);

    //split data 
    mydata_t dataLeft, dataRight;
    dataLeft.Count = data.Count / 2;
    dataRight.Count = data.Count - dataLeft.Count;
    dataLeft.lines = malloc((dataLeft.Count) * sizeof(char *));
    dataRight.lines = malloc((dataRight.Count) * sizeof(char *));


    if (!dataLeft.lines || !dataRight.lines) { //initialized properly?
        perror("malloc\n");
        free(data.lines);
        free(dataLeft.lines);
        free(dataRight.lines);
        exit(EXIT_FAILURE);
    }

    //copy data into new left and right arrays
    for(u_int i = 0; i < dataLeft.Count; i++){
        dataLeft.lines[i] = data.lines[i];
    }
    for(u_int i = 0; i < dataRight.Count; i++){
        dataRight.lines[i] = data.lines[dataLeft.Count + i];
    }

    //free original data, new arrays contain data
    free(data.lines);

    //pipe data
    int pipeLeft[2], pipeRight[2];
    if(pipe(pipeLeft) == -1 || pipe(pipeRight) == -1){ //create the pipes
        perror("error creating pipes\n");
        free(dataLeft.lines);
        free(dataRight.lines);
        exit(EXIT_FAILURE);
    }

    //left 
    pid_t leftChild = fork();

    if(leftChild == -1){
        fprintf(stderr, "error in forking left child\n");
        free(dataLeft.lines);
        free(dataRight.lines);
        exit(EXIT_FAILURE);
    }

    if(leftChild == 0){ //child left
       
        close(pipeLeft[0]);
        //dup2(pipeLeft[0], STDIN_FILENO);
        dup2(pipeLeft[1], STDOUT_FILENO);   //redirect stdout to pipe
        close(pipeLeft[1]);
        
        //recursion
        execlp(argv[0], argv[0], NULL);
    

        //if execlp fails
        perror("execlp left failed\n");
        free(data.lines);
        free(dataLeft.lines);
        free(dataRight.lines);
        exit(EXIT_FAILURE);
    }

    //right
    pid_t rightChild = fork();

    if(rightChild == -1){
        fprintf(stderr, "error in forking left child\n");
        free(data.lines);
        free(dataLeft.lines);
        free(dataRight.lines);
        exit(EXIT_FAILURE);
    }

    if(rightChild == 0){ //child right
        

        close(pipeRight[0]);    //close read end of right pipe
        //dup2(pipeRight[0], STDIN_FILENO);
        dup2(pipeRight[1], STDOUT_FILENO);   //redirect stdout to pipe

        close(pipeRight[1]);

        //recursion
        execlp(argv[0], argv[0], NULL);
        //execlp(argv[0], argv[0], "right", NULL);

        //if execlp fails
        perror("execlp left failed\n");
        free(data.lines);
        free(dataLeft.lines);
        free(dataRight.lines);
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
        free(dataLeft.lines);
        free(dataRight.lines);
        exit(EXIT_FAILURE);
    }
   
    FILE *rightIn = fdopen(pipeRight[1], "w");
    if (rightIn == NULL) {
        perror("Error opening pipe for writing (right)\n");
        free(dataLeft.lines);
        free(dataRight.lines);
        exit(EXIT_FAILURE);
    }

    for(int i = 0; i < dataLeft.Count; i++){
        fprintf(leftIn, "%s", dataLeft.lines[i]);
    }

    for(int i = 0; i < dataRight.Count; i++){
        fprintf(rightIn, "%s", dataRight.lines[i]);
    }

    free(dataLeft.lines);
    free(dataRight.lines);
    fclose(leftIn);
    fclose(rightIn);

    close(pipeLeft[1]);        
    close(pipeRight[1]); 

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
        free(data.lines);
        free(dataLeft.lines);
        free(dataRight.lines);
        exit(EXIT_FAILURE);
    }
    
    
    char **sortedLeft = readInput(leftStream, &dataLeft.Count);
    char **sortedRight = readInput(rightStream, &dataRight.Count);

    fclose(leftStream);
    fclose(rightStream);


    close(pipeLeft[0]);        
    close(pipeRight[0]); 


    //merge
    merge(stdout, sortedLeft, sortedRight, dataLeft.Count, dataRight.Count);

    //Free all memory

    //close(pipeLeft[0]);    
    //close(pipeLeft[1]);    
    //close(pipeRight[0]);    
    //close(pipeRight[1]);  

    for (unsigned int i = 0; i < dataLeft.Count; i++) {
        free(sortedLeft[i]);
    }
    free(sortedLeft);

    for (unsigned int i = 0; i < dataRight.Count; i++) {
        free(sortedRight[i]);
    }
    free(sortedRight);


    //free the rest
    for (unsigned int i = 0; i < dataLeft.Count; i++) {
        free(dataLeft.lines[i]);
    }
    free(dataLeft.lines);

    for (unsigned int i = 0; i < dataRight.Count; i++) {
        free(dataRight.lines[i]);
    }
    free(dataRight.lines);


    return EXIT_SUCCESS;;
}