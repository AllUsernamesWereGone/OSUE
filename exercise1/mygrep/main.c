#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

#include "mygrep.h"

int main (int argc, char** argv){
    
    /**
     * Input should look like: 
     * mygrep [-i] [-o outfile] keyword [file...]
     *
     * if -i then: ignore lower / uppercase 
     * if -o then: write the output to specified file
     * multiple files are allowed as input, to be read in order that is given
     * following characters are allowed: [0-9][A-Z][a-z].,:-!=?% and whitespace
     * if succesful terminate with: EXIT_SUCCESS
     * if unsuccesfull: EXIT_FAILURE
     */

    if(argc <=1){
        return printUsage(argv[0]);
    }

    bool ignoreUpperLowerCase = false;
    char* outfileName = NULL;
    FILE* infile = stdin;
    FILE* outputFile = stdout;
    char* keyword = NULL;

    int opt;
    while((opt = getopt(argc, argv, "io:"))!=-1){
        switch(opt){
            case 'i':
                if(ignoreUpperLowerCase){
                    fprintf(stderr, "Error: Option -i specified multiple times.\n");
                    return printUsage(argv[0]);
                }
                ignoreUpperLowerCase = true;
                break;
            case 'o':
                if(outfileName){
                    fprintf(stderr, "Error: Option -o specified multiple times.\n");
                    return printUsage(argv[0]);
                }
                outfileName = optarg;
                break;
            default: 
                fprintf(stderr, "Error: Unrecognized option2 %s.\n", argv[0]);
                return printUsage(argv[0]);
                
        }
    }

    if(optind >= argc){
        fprintf(stderr, "Error: No keyword specified.\n");
        return printUsage(argv[0]);
    }else{
        keyword = argv[optind];
        optind++;
    }


    if(!keyword){
        fprintf(stderr, "Error: No keyword specified.\n");
        return printUsage(argv[0]);
    }
  
    //falls es in ein file geschrieben werden soll
    if(outfileName){
        outputFile = fopen(outfileName, "w");
        if(!outputFile){
            fprintf(stderr, "Error opening specified output file.\n");
            return EXIT_FAILURE;
        }
    }

    //read from files
    if(optind==argc){
        searchFile(stdin, keyword, ignoreUpperLowerCase, outputFile);
    }else{ //input file(s) specified
        for(int i = optind; i< argc; i++){    
            infile = fopen(argv[i], "r");
            if(!infile){
                fprintf(stderr, "Error opening file: %s.\n", argv[i]);
                return EXIT_FAILURE;
            }
            searchFile(infile, keyword, ignoreUpperLowerCase, outputFile);
            fclose(infile);
        }   
    }

    //close output file if existed
    if(outfileName){
        fclose(outputFile);
    }

//    printf("SUCCESS\n");
    return EXIT_SUCCESS;
}
