#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>

#include "models.h"
#include "shapes.h"
#include "camera.h"
#include "transforms.h"


int main() {

    // Start measuring time
    struct timeval begin, end;
    gettimeofday(&begin, 0);

    // CODE 

    uint16_t w = 6;
    uint16_t h = 6;
    
    char values[] = {'.',':','i','l','w','W'};

    for(uint16_t y = 0; y < h; y++) {
        for(uint16_t x = 0; x < w; x++) {
            printf("%c%c ", values[y], values[x]);
        }   
        puts(" ");
    }

    // END CODE


    // Stop measuring time and calculate the elapsed time
    gettimeofday(&end, 0);
    long seconds = end.tv_sec - begin.tv_sec;
    long microseconds = end.tv_usec - begin.tv_usec;
    double elapsed = seconds + microseconds*1e-6;
    
    printf("Time measured: %.5f seconds.\n", elapsed);
	return 0;
}