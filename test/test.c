#include "models.h"

int main() {
	// ModelData ammoBox = load_model_from_path("./assets/ammo_box_1_1.obj", "./assets/ammo_mp_1.png", true);
	// free_model(&ammoBox);
	
    ModelData teapot = load_model_from_path("./assets/newell_teaset/teapot.obj", NULL, false);
	free_model(&teapot);
	return 0;
}