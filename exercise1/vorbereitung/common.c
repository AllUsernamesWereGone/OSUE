#include "common.h"


void wait_ms(int ms){
    clock_t start_time = clock();
    while (clock() < start_time + ms*1000);
}