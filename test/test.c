#include <stdio.h>
#include "models.h"

int main() {
	// ModelData ammoBox = load_model_from_path("./assets/ammo_box_1_1.obj", "./assets/ammo_mp_1.png", true);
	// free_model(&ammoBox);
	
    // ModelData teapot = load_model_from_path("./assets/newell_teaset/teapot.obj", NULL, true);
    // ModelData teacup = load_model_from_path("./assets/newell_teaset/teacup.obj", NULL, true);
    // ModelData spoon = load_model_from_path("./assets/newell_teaset/spoon.obj", NULL, true);
    ModelData cube = load_model_from_path("./assets/cube.obj", NULL, false, false);

    printf("%ld %ld %ld %ld \n", cube.vertsCount, cube.trisCount, cube.normalsCount, cube.uvsCount);

    for(size_t i = 0; i < cube.uvsCount; i++) {
        print_vec3("uvs:", cube.uvs[i]);
    }

    // free_model(&teapot);
    // free_model(&teacup);
    // free_model(&spoon);
    free_model(&cube);

	return 0;
}