#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>


#include "raylib.h"
#include "transforms.h"
#include "camera.h"
#include "models.h"
#include "render.h"
#include "scene.h"
#include "shapes.h"

#define WIDTH  800
#define HEIGHT 450
#define TARGET_FPS 60

void move_cube(Instance *instance, float delta) {
    if (IsKeyDown(KEY_I)) {
        instance->transforms.position.z += .5f*delta;
        update_instance_transforms(instance);
    }
    
    if (IsKeyDown(KEY_K)) {
        instance->transforms.position.z -= .5f*delta;
        update_instance_transforms(instance);
    }

    if (IsKeyDown(KEY_J)) {
        instance->transforms.position.x -= .5f*delta;
        update_instance_transforms(instance);
    }
    
    if (IsKeyDown(KEY_L)) {
        instance->transforms.position.x += .5f*delta;
        update_instance_transforms(instance);
    }
    
    if (IsKeyDown(KEY_U)) {
        instance->transforms.rotation.y += 15.0f*delta;
        update_instance_transforms(instance);
    }
    
    if (IsKeyDown(KEY_O)) {
        instance->transforms.rotation.y -= 15.0f*delta;
        update_instance_transforms(instance);
    }

    if (IsKeyDown(KEY_N)) {
        instance->transforms.rotation.z += 15.0f*delta;
        update_instance_transforms(instance);
    }
    
    if (IsKeyDown(KEY_M)) {
        instance->transforms.rotation.z -= 15.0f*delta;
        update_instance_transforms(instance);
    }
}

int main(void)
{
    float moveSpeed = 2.0f;
    float turnSpeed = 40.0f;

    Cam camera = init_camera(
        WIDTH, HEIGHT, 
        // (Vec3) {.x = 0.0f, .y = 0.0f, .z = 0.0f},
        // (Vec3) {.x = 0.0f, .y = 0.0f, .z = 0.0f}
        (Vec3) {.x = .93f, .y = 1.43f, .z = .16f},
        (Vec3) {.x = -21.01f, .y = 43.65f, .z = 0}
    );

    ModelData teapot = load_model_from_path("./assets/newell_teaset/teapot.obj", "./assets/psx_pizza_doggy/metal_mp_3.png", false, false, true);
    ModelData teacup = load_model_from_path("./assets/newell_teaset/teacup.obj", "./assets/psx_pizza_doggy/plastic_3.png", false, false, true);
    ModelData spoon = load_model_from_path("./assets/newell_teaset/spoon.obj", "./assets/psx_pizza_doggy/metal_mp_3.png", false, false, true);
    
    ModelData floor = load_model_from_path(
        "./assets/psx_pizza_doggy/floor_ceiling_1.obj", "./assets/psx_pizza_doggy/floor_1.png", false, false, true);
    
    ModelData ammo = load_model_from_path(
        "./assets/psx_pizza_doggy/ammo_box_1_1.obj", "./assets/psx_pizza_doggy/ammo_mp_1.png", false, false, true);
    
    ModelData ammoPistol = load_model_from_path(
        "./assets/psx_pizza_doggy/ammo_pistol_loose_5pcs_mp_1.obj", "./assets/psx_pizza_doggy/ammo_mp_1.png", false, false, true);
    
    ModelData chair = load_model_from_path(
        "./assets/psx_pizza_doggy/chair_mp_3.obj", "./assets/psx_pizza_doggy/wood_3.png", false, false, true);
    
    ModelData flashlight = load_model_from_path(
        "./assets/psx_pizza_doggy/flashlight_1.obj", "./assets/psx_pizza_doggy/flashlight_1.png", false, false, true);
    
    ModelData matchbox = load_model_from_path(
        "./assets/psx_pizza_doggy/matchbox_2.obj", "./assets/psx_pizza_doggy/matchbox_1.png", false, false, true);
    
    ModelData keyboard = load_model_from_path(
        "./assets/psx_pizza_doggy/pc_keyboard_mp_3.obj", "./assets/psx_pizza_doggy/pc_accessories_mp_1.png", false, false, true);
    
    ModelData monitor = load_model_from_path(
        "./assets/psx_pizza_doggy/pc_monitor_mp_1.obj", "./assets/psx_pizza_doggy/pc_monitor_mp_1.png", false, false, true);
    
    ModelData pistolMag = load_model_from_path(
        "./assets/psx_pizza_doggy/pistol_mp_1_mag_empty.obj", "./assets/psx_pizza_doggy/pistols_mp_1.png", false, false, true);
    
    ModelData table = load_model_from_path(
        "./assets/psx_pizza_doggy/table_large_2.obj", "./assets/psx_pizza_doggy/wood_3.png", false, false, true);
    
    ModelData vhs = load_model_from_path(
        "./assets/psx_pizza_doggy/vhs_tape_1.obj", "./assets/psx_pizza_doggy/vhs_tape_1.png", false, false, true);
    
    ModelData marbleBust = load_model_from_path(
        "./assets/polyhaven_rico_b3d/marble_bust.obj", "./assets/polyhaven_rico_b3d/marble_bust.jpg", false, false, true);
   
    // TODO: Render tranparent objects last
    // ModelData fakeGodRays = load_model_from_path(
    //     "./assets/psx_pizza_doggy/fake_light_window_2_double.obj", "./assets/psx_pizza_doggy/fake_light_mp_1.png", false, false, true);

    // TODO: better instance loading
    Instance instances[] = {
        init_instance(&marbleBust, (Transforms){
            .position = (Vec3){.x = -1.3, .y = 0.0f, .z = .6f},
            .rotation = (Vec3){.x = 0.0f, .y = 0.0f, .z = 0.0f},
            .scale = (Vec3){.x = 1.0f, .y = 1.0f, .z = 1.0f}
        }),
        init_instance(&floor, (Transforms){
            .position = (Vec3){.x = 0.0f, .y = 0.0f, .z = 1.0f},
            .rotation = (Vec3){.x = 0.0f, .y = 0.0f, .z = 0.0f},
            .scale = (Vec3){.x = 1.0f, .y = 1.0f, .z = 1.0f}
        }),
        init_instance(&spoon, (Transforms){
            .position = (Vec3){.x = 0.2f, .y = .97f, .z = 1.0f},
            .rotation = (Vec3){.x = 10.0f, .y = 0.0f, .z = 0.0f},
            .scale = (Vec3){.x = 1.0f/8.0f, .y = 1.0f/8.0f, .z = 1.0f/8.0f}
        }),
        init_instance(&teacup, (Transforms){
            .position = (Vec3){.x = -0.4f, .y = .97f, .z = 0.9f},
            .rotation = (Vec3){.x = 0.0f, .y = 0.0f, .z = 0.0f},
            .scale = (Vec3){.x = 1.0f/8.0f, .y = 1.0f/8.0f, .z = 1.0f/8.0f}
        }),
        init_instance(&teapot, (Transforms) {
            .position = (Vec3){.x = -1.0, .y = 0.0f, .z = 1.0f},
            .rotation = (Vec3){.x = 0.0f, .y = 0.0f, .z = 0.0f},
            .scale = (Vec3){.x = 1.0f/9.0f, .y = 1.0f/8.0f, .z = 1.0f/8.0f},
        }),
        init_instance(&ammo, (Transforms){
            .position = (Vec3){.x = -.07f, .y = .97f, .z = 1.0f},
            .rotation = (Vec3){.x = 0.0f, .y = -45.0f, .z = 0.0f},
            .scale = (Vec3){.x = 1.0f, .y = 1.0f, .z = 1.0f}
        }),
        init_instance(&ammoPistol, (Transforms){
            .position = (Vec3){.x = .1f, .y = .97f, .z = 1.0f},
            .rotation = (Vec3){.x = 0.0f, .y = 0.0f, .z = 0.0f},
            .scale = (Vec3){.x = 1.0f, .y = 1.0f, .z = 1.0f}
        }),
        init_instance(&chair, (Transforms){
            .position = (Vec3){.x = 0.0f, .y = 0.0f, .z = 0.5f},
            .rotation = (Vec3){.x = 0.0f, .y = -200.0f, .z = 0.0f},
            .scale = (Vec3){.x = 1.0f, .y = 1.0f, .z = 1.0f}
        }),
        init_instance(&flashlight, (Transforms){
            .position = (Vec3){.x = -.6f, .y = .97f, .z = 1.3f},
            .rotation = (Vec3){.x = 0.0f, .y = -160.0f, .z = 0.0f},
            .scale = (Vec3){.x = 1.0f, .y = 1.0f, .z = 1.0f}
        }),
        init_instance(&matchbox, (Transforms){
            .position = (Vec3){.x = -.6f, .y = .97f, .z = .64f},
            .rotation = (Vec3){.x = 0.0f, .y = 40.0f, .z = 0.0f},
            .scale = (Vec3){.x = 1.0f, .y = 1.0f, .z = 1.0f}
        }),
        init_instance(&keyboard, (Transforms) {
            .position = (Vec3){.x = 0.5f, .y = .97f, .z = .9f},
            .rotation = (Vec3){.x = 0.0f, .y = -20.0f, .z = 0.0f},
            .scale = (Vec3){.x = 1.0f, .y = 1.0f, .z = 1.0f},
        }),
        init_instance(&monitor, (Transforms) {
            .position = (Vec3){.x = 0.6f, .y = .97f, .z = 1.2f},
            .rotation = (Vec3){.x = 0.0f, .y = -30.0f, .z = 0.0f},
            .scale = (Vec3){.x = 1.0f, .y = 1.0f, .z = 1.0f},
        }),
        init_instance(&pistolMag, (Transforms){
            .position = (Vec3){.x = .24f, .y = .99f, .z = .7f},
            .rotation = (Vec3){.x = 90.0f, .y = 90.0f, .z = 0.0f},
            .scale = (Vec3){.x = 1.0f, .y = 1.0f, .z = 1.0f}
        }),
        init_instance(&table, (Transforms){
            .position = (Vec3){.x = 0.0f, .y = 0.0f, .z = 1.0f},
            .rotation = (Vec3){.x = 0.0f, .y = 0.0f, .z = 0.0f},
            .scale = (Vec3){.x = 1.0f, .y = 1.0f, .z = 1.0f}
        }),
        init_instance(&vhs, (Transforms){
            .position = (Vec3){.x = -.05f, .y = .97f, .z = 1.24f},
            .rotation = (Vec3){.x = 0.0f, .y = -62.0f, .z = 0.0f},
            .scale = (Vec3){.x = 1.0f, .y = 1.0f, .z = 1.0f}
        }),
        // init_instance(&fakeGodRays, (Transforms) {
        //     .position = (Vec3){.x = 0.25, .y = 1.5f, .z = 10.0f},
        //     .rotation = (Vec3){.x = 0.0f, .y = 95.0f, .z = 0.0f},
        //     .scale = (Vec3){.x = 1.0f, .y = 1.0f, .z = 1.0f},
        // })
    };

    // TODO: Proper scene builder
    Scene scene = (Scene) {
        .instances = instances,
        .objectCount = sizeof(instances) / sizeof(instances[0])
    };
    
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_ALWAYS_RUN | FLAG_MSAA_4X_HINT);
    InitWindow(camera.width, camera.height, "raylib [core] example - basic window");
    
    Image img = GenImageColor(camera.width, camera.height, RAYWHITE);
    Texture2D renderTexture = LoadTextureFromImage(img);
    
    SetTargetFPS(TARGET_FPS);
    // goto closeWindow;
    
    while (!WindowShouldClose())
    {
        clear_canvas(camera);

        float delta = GetFrameTime();
        move_cube(&instances[0], delta);

        float deltaMoveSpeed = moveSpeed * delta;
        float deltaTurnSpeed = turnSpeed * delta;

        if (IsKeyDown(KEY_W)) {
            camera_move_forward(&camera, deltaMoveSpeed);
        }
        
        if (IsKeyDown(KEY_S)) {
            camera_move_backward(&camera, deltaMoveSpeed);
        }

        if (IsKeyDown(KEY_A)) {
            camera_move_left(&camera, deltaMoveSpeed);
        }
        
        if (IsKeyDown(KEY_D)) {
            camera_move_right(&camera, deltaMoveSpeed);
        }
        
        if (IsKeyDown(KEY_SPACE)) {
            camera_move_up(&camera, deltaMoveSpeed);
        }
        
        if (IsKeyDown(KEY_LEFT_CONTROL)) {
            camera_move_down(&camera, deltaMoveSpeed);
        }

        if (IsKeyDown(KEY_UP)) {
            camera_turn_up(&camera, deltaTurnSpeed);
        }
        
        if (IsKeyDown(KEY_DOWN)) {
            camera_turn_down(&camera, deltaTurnSpeed);
        }

        if (IsKeyDown(KEY_LEFT)) {
            camera_turn_left(&camera, deltaTurnSpeed);
        }
        
        if (IsKeyDown(KEY_RIGHT)) {
            camera_turn_right(&camera, deltaTurnSpeed);
        }

        if (IsKeyPressed(KEY_Z)) {
            camera.renderDepth = !camera.renderDepth;
        }

        if (IsKeyPressed(KEY_X)) {
            camera.wireFrame = !camera.wireFrame;
        }

        // TODO: Use mouse to look around
        // Vector2 mouse = GetMouseDelta();
        // printf("mouse x:%+03.02f y:%+03.02f\n",mouse.x, mouse.y);

        if (IsWindowResized()) {
            update_camera_frustum(&camera, GetScreenWidth(), GetScreenHeight());
            if (camera.canvas == NULL) {
                goto closeWindow;
            }
            
            ImageResize(&img, camera.width, camera.height);
            UnloadTexture(renderTexture);            
            renderTexture = LoadTextureFromImage(img);
        }

        // struct timeval begin, end;
        // gettimeofday(&begin, 0);

        render_scene(camera, scene);

        // Stop measuring time and calculate the elapsed time
        // gettimeofday(&end, 0);
        // long seconds = end.tv_sec - begin.tv_sec;
        // long microseconds = end.tv_usec - begin.tv_usec;
        // double elapsed = seconds + microseconds*1e-6;
        // printf("Time measured: %.5f seconds.\n", elapsed);


        UpdateTexture(renderTexture, camera.canvas);
        
        BeginDrawing();
            ClearBackground(RAYWHITE);

            DrawTexture(renderTexture, 0, 0, RAYWHITE);
            DrawText(
                TextFormat(
                    "X: %.2f Y: %.2f Z: %.2f", 
                    camera.position.x, camera.position.y, camera.position.z), 
                15, 40, 20, RAYWHITE);
            DrawText(
                TextFormat(
                    "X: °%.2f Y: °%.2f Z: °%.2f", 
                    camera.rotation.x, camera.rotation.y, camera.rotation.z), 
                15, 60, 20, RAYWHITE);
            DrawFPS(15,15);
        EndDrawing();
        // break;
    }

closeWindow:
    for (size_t i = 0; i < scene.objectCount; i++) {
        free_model(scene.instances[i].model);
        free_instance(&scene.instances[i]);
    }
    free_camera(camera);
    UnloadImage(img);
    UnloadTexture(renderTexture);

    CloseWindow();    
    return 0;
}