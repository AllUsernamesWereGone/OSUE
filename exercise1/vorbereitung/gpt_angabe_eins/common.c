#include "common.h"


void success(char *msg){
    printf("%s\n", msg);
}
void error_exit(char *msg){
    printf("%s\n", msg);
    exit(EXIT_FAILURE);
}
int calc_input(int number){
    return number *2;
}