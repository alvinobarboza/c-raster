#include <stdio.h>
#include <sys/time.h>

#include "models.h"
#include "shapes.h"

int main() {

    // Start measuring time
    struct timeval begin, end;
    gettimeofday(&begin, 0);

	// ModelData ammoBox = load_model_from_path("./assets/ammo_box_1_1.obj", "./assets/ammo_mp_1.png", true);
	// free_model(&ammoBox);
	
    // ModelData teapot = load_model_from_path("./assets/newell_teaset/teapot.obj", NULL, true);
    // ModelData teacup = load_model_from_path("./assets/newell_teaset/teacup.obj", NULL, true);
    // ModelData spoon = load_model_from_path("./assets/newell_teaset/spoon.obj", NULL, true);
    // ModelData cube = load_model_from_path("./assets/cube.obj", NULL, false, false);
    ModelData triangle = triangle_shape();

    // printf("%ld %ld %ld %ld \n", cube.vertsCount, cube.trisCount, cube.normalsCount, cube.uvsCount);

    // for(size_t i = 0; i < cube.uvsCount; i++) {
    //     print_vec3("uvs:", cube.uvs[i]);
    // }

    // free_model(&teapot);
    // free_model(&teacup);
    // free_model(&spoon);
    // free_model(&cube);
    free_model(&triangle);


    // Stop measuring time and calculate the elapsed time
    gettimeofday(&end, 0);
    long seconds = end.tv_sec - begin.tv_sec;
    long microseconds = end.tv_usec - begin.tv_usec;
    double elapsed = seconds + microseconds*1e-6;
    
    printf("Time measured: %.5f seconds.\n", elapsed);
	return 0;
}