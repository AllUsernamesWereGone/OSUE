#include "mygrep.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

//ab hier beginnt der code
static void convertToLower(char *str) {
   while (*str) {
      *str = tolower(*str);
      str++;
   }
}

int printUsage(char* programName){
    fprintf(stderr, "Usage: %s [-i] [-o outfile] keyword [file...]\n", programName);
    return EXIT_FAILURE;
}

void searchFile(FILE *infile, char *keyword, bool ignoreCase, FILE *outfile){

    //länge hier müsste variable sein, aber wie?!
    char buffer[(infile == stdin) ? 1024 : sizeof(infile)];
    char bufferLower[(infile == stdin) ? 1024 : sizeof(infile)];
    
    
    char keywordNew[sizeof(keyword)];

    if(ignoreCase){//case insensitive search A==a
        //create lowercase keyword 
        strcpy(keywordNew, keyword);
        convertToLower(keywordNew);
    }else{
        strcpy(keywordNew, keyword);
    }

    while(fgets(buffer, sizeof(buffer), infile)){
        strcpy(bufferLower, buffer);
        if(ignoreCase){
            convertToLower(bufferLower);
        }

        if(strstr(bufferLower,keywordNew)!= NULL){
            fprintf(outfile,"%s", buffer);
        }           
    }
}


