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

    ModelData cube = load_model_from_path("./assets/cube.obj", NULL, false, false);

    printf("texture: w->%d h->%d\n", cube.texture->width, cube.texture->height);

    for(uint16_t y = 0; y < cube.texture->height; y++) {
        if (y % 10 == 0) {
            bool skip = false;
            for(uint16_t x = 0; x < cube.texture->width; x++) {
                if (x % 10 == 0)  {
                    printf("%c ", cube.texture->colors[y*cube.texture->width+x].r == 80 ? '.' : '0');
                    skip = true;
                }
            }
            if (skip) puts(" ");
        }
    }

    free_model(&cube);

    // END CODE


    // Stop measuring time and calculate the elapsed time
    gettimeofday(&end, 0);
    long seconds = end.tv_sec - begin.tv_sec;
    long microseconds = end.tv_usec - begin.tv_usec;
    double elapsed = seconds + microseconds*1e-6;
    
    printf("Time measured: %.5f seconds.\n", elapsed);
	return 0;
}