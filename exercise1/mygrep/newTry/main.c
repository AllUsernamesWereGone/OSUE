#include "mygrep.h"
//git test
//nicht da sein
int  errorMSG(){
    fprintf(stderr, "Error Code: 0815\nUsage: mygrep [-i] [-o outfile] keyword [file...]\n");
    return (EXIT_FAILURE);
}


int main(int argc, char** argv){

    //printf("argc: %d\nargv: %s\n", argc, argv[0]);

    if(argc < 2){//zu wenig argumente
        exit(errorMSG());
    }
    
    int ignoreCase = 0;
    int outfile = -1;

    int opt = 0;
    while ((opt = getopt(argc, argv, "io:")) != -1) {
        switch (opt) {
        case 'i':
            if(ignoreCase){
                return errorMSG();
            }
            ignoreCase = 1;
            break;
        case 'o':
            if(outfile != -1){
                return errorMSG();
            }
            outfile = atoi(optarg);
            break;
        default: /* '?' */
            return errorMSG();
        }
    }

    if (optind >= argc) {
        fprintf(stderr, "Expected keyword after options\n");
        return errorMSG();
    }else{
        char* keyword = argv[optind];
        printf("name argument = %s\n", keyword);
        optind++;
    }

    
    

    if(optind == argc){ //read from stdin if no input files specified
        //my function to read in stuff from stdin

        
        printf("stdin name read in from = %s\n", argv[optind]);
    }else{//read from files in order
        for(int i = optind; i < argc; i++){
            //my function to read in stuff from infiles argv[i]



            printf("bbbbb name read in from = %s\n", argv[i]);
        }
    }









    return EXIT_SUCCESS;



}//end main




