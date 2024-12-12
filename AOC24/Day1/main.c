#include <stdio.h>
#include <stdlib.h>

const int size = 1000;

void parsefile(int *l, int *r){
    FILE *fp;                       //file pointer
    fp = fopen("input.txt", "r");   //reads from file

    char num[100] = "a";

    for(int i = 0; i < size; i++){ //befülle beide arrays
        fgets(num, 9, fp);
        l[i] = atoi(num);
        fgets(num, 9, fp);
        r[i] = atoi(num);
    }
}

int compare( const void* a, const void* b){
    if( *(int*)a == *(int*)b ) return 0;
    return *(int*)a < *(int*)b ? -1 : 1;
}


int main(int argc, char** argv){


    int numbers_left [size];        //1000 rows
    int numbers_right [size];       //1000 rows

    parsefile(numbers_left, numbers_right);

    /*
    printf("left: %d   right: %d\n", numbers_left[0], numbers_right[0]);
    printf("left: %d   right: %d\n", numbers_left[1], numbers_right[1]);
    printf("left: %d   right: %d\n------------\n", numbers_left[2], numbers_right[2]);
    */

    qsort( numbers_left, 1000, sizeof(int), compare );
    qsort( numbers_right, 1000, sizeof(int), compare );

    /*
    printf("left: %d   right: %d\n", numbers_left[0], numbers_right[0]);
    printf("left: %d   right: %d\n", numbers_left[1], numbers_right[1]);
    printf("left: %d   right: %d\n", numbers_left[2], numbers_right[2]);
    */
    int sum = 0;
    int value = 0;
    for(int i = 0; i < size; i++){

        value = numbers_right[i] - numbers_left[i];
        if(value < 0){
            value = value * (-1);
        }
        sum += value;
        
    }
    printf("Teil 1, sum: %i\n", sum);

    sum = 0;
    value = 0;
    int counter = 0;

    for(int i = 0; i < size; i++){
        for(int j = 0; j < size; j++){
            if(numbers_left[i] < numbers_right[j]){
                continue;
            }
            if(numbers_left[i] == numbers_right[j]){
                //Berechnung
                value = numbers_right[j];
                counter ++;
            }
            if(numbers_left[i] > numbers_right[j]){
                sum = sum + (value * counter);
                counter =  0;
                value = 0;
            }
        }
    }

    printf("Teil 2, sum: %i\n", sum);

    return EXIT_SUCCESS;
}