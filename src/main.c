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

#define WIDTH  1600
#define HEIGHT 900
#define TARGET_FPS 60

void move_cube(Instance *instance) {
    if (IsKeyDown(KEY_I)) {
        instance->transforms.position.z += .5f;
        update_instance_transforms(instance);
    }
    
    if (IsKeyDown(KEY_K)) {
        instance->transforms.position.z -= .5f;
        update_instance_transforms(instance);
    }

    if (IsKeyDown(KEY_J)) {
        instance->transforms.position.x -= .5f;
        update_instance_transforms(instance);
    }
    
    if (IsKeyDown(KEY_L)) {
        instance->transforms.position.x += .5f;
        update_instance_transforms(instance);
    }
    
    if (IsKeyDown(KEY_U)) {
        instance->transforms.rotation.z += .5f;
        update_instance_transforms(instance);
    }
    
    if (IsKeyDown(KEY_O)) {
        instance->transforms.rotation.z -= .5f;
        update_instance_transforms(instance);
    }
}

int main(void)
{
    float moveSpeed = 5.0f;
    float turnSpeed = 60.0f;

    Cam camera = init_camera(
        WIDTH, HEIGHT, 
        (Vec3) {.x = 0.0f, .y = 0.0f, .z = 3.0f},
        (Vec3) {.x = 0.0f, .y = 0.0f, .z = 0.0f}
        // (Vec3) {.x = 1.69f, .y = 0.38f, .z = 3.06f},
        // (Vec3) {.x = 11.27f, .y = 92.73f, .z = 0}
    );

    ModelData teapot = load_model_from_path("./assets/newell_teaset/teapot.obj", NULL, false, false);
    ModelData teacup = load_model_from_path("./assets/newell_teaset/teacup.obj", NULL, false, false);
    ModelData spoon = load_model_from_path("./assets/newell_teaset/spoon.obj", NULL, false, false);
    ModelData cube = load_model_from_path("./assets/cube.obj", NULL, false, false);

    ModelData icosahedron = icosahedron_shape();
    ModelData torus = torus_shape();
    ModelData uvSphere = uv_sphere_shape();
    ModelData triangle = triangle_shape();

    // TODO: better instance loading
    Instance instances[] = {
        init_instance(&triangle, (Transforms){
            .position = (Vec3){.x = 0.0f, .y = 0.0f, .z = 6.0f},
            .rotation = (Vec3){.x = 0.0f, .y = 0.0f, .z = 0.0f},
            .scale = (Vec3){.x = 1.0f, .y = 1.0f, .z = 1.0f}
        }),
        init_instance(&cube, (Transforms) {
            .position = (Vec3){.x = 1.0f, .y = 0.0f, .z = 6.5f},
            .rotation = (Vec3){.x = 0.0f, .y = 0.0f, .z = 0.0f},
            .scale = (Vec3){.x = 1.0f, .y = 1.0f, .z = 1.0f},
        }),
        init_instance(&spoon, (Transforms){
            .position = (Vec3){.x = 0.0f, .y = 0.0f, .z = 4.0f},
            .rotation = (Vec3){.x = 0.0f, .y = 0.0f, .z = 0.0f},
            .scale = (Vec3){.x = 1.0f, .y = 1.0f, .z = 1.0f}
        }),
        init_instance(&teacup, (Transforms){
            .position = (Vec3){.x = 0.0f, .y = 0.0f, .z = 2.0f},
            .rotation = (Vec3){.x = 0.0f, .y = 0.0f, .z = 0.0f},
            .scale = (Vec3){.x = 1.0f, .y = 1.0f, .z = 1.0f}
        }),
        init_instance(&teapot, (Transforms) {
            .position = (Vec3){.x = -2.5, .y = 0.0f, .z = 5.0f},
            .rotation = (Vec3){.x = 0.0f, .y = 0.0f, .z = 0.0f},
            .scale = (Vec3){.x = 1.0f, .y = 1.0f, .z = 1.0f},
        }),
        init_instance(&icosahedron, (Transforms) {
            .position = (Vec3){.x = 0.25, .y = 1.5f, .z = 20.0f},
            .rotation = (Vec3){.x = 0.0f, .y = 95.0f, .z = 0.0f},
            .scale = (Vec3){.x = 1.0f, .y = 1.0f, .z = 1.0f},
        }),
        init_instance(&torus, (Transforms) {
            .position = (Vec3){.x = 8.25, .y = -1.5f, .z = 15.0f},
            .rotation = (Vec3){.x = 0.0f, .y = -45.0f, .z = 0.0f},
            .scale = (Vec3){.x = 2.0f, .y = 2.0f, .z = 2.0f},
        }),
        init_instance(&uvSphere, (Transforms) {
            .position = (Vec3){.x = -0.25, .y = -1.5f, .z = 15.0f},
            .rotation = (Vec3){.x = 0.0f, .y = 0.0f, .z = 0.0f},
            .scale = (Vec3){.x = 2.0f, .y = 2.0f, .z = 2.0f},
        })
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

        move_cube(&instances[0]);
        float delta = GetFrameTime();

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