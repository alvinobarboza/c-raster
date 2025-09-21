#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "raylib.h"
#include "transforms.h"
#include "camera.h"
#include "models.h"
#include "render.h"
#include "scene.h"

#define WIDTH  1000
#define HEIGHT 400
#define TARGET_FPS 60

void move_cube(Instance *instance) {
    if (IsKeyDown(KEY_I)) {
        instance->position.z += .5f;
        update_instance_transforms(instance);
    }
    
    if (IsKeyDown(KEY_K)) {
        instance->position.z -= .5f;
        update_instance_transforms(instance);
    }

    if (IsKeyDown(KEY_J)) {
        instance->position.x -= .5f;
        update_instance_transforms(instance);
    }
    
    if (IsKeyDown(KEY_L)) {
        instance->position.x += .5f;
        update_instance_transforms(instance);
    }
    
    if (IsKeyDown(KEY_U)) {
        instance->rotation.y += .5f;
        update_instance_transforms(instance);
    }
    
    if (IsKeyDown(KEY_O)) {
        instance->rotation.y -= .5f;
        update_instance_transforms(instance);
    }
}

int main(void)
{
    float moveSpeed = 15.0f;
    float turnSpeed = 70.0f;

    Cam camera = init_camera(
        WIDTH, HEIGHT, 
        (Vec3) {.x = -3.0f, .y = 1.0f, .z = 2.0f},
        (Vec3) {.x = 0.0f, .y = -30.0f, .z = 0.0f}
    );

    ModelData cube = cube_model();

    Instance instances[2] = {
        (Instance) {
            .model = &cube,
            .position = (Vec3){.x = -2.5, .y = 0.0f, .z = 10.0f},
            .rotation = (Vec3){.x = 0.0f, .y = 0.0f, .z = 0.0f},
            .scale = (Vec3){.x = .7f, .y = .7f, .z = .7f},
        },
        (Instance) {
            .model = &cube,
            .position = (Vec3){.x = 2.25, .y = 2.5f, .z = 9.5f},
            .rotation = (Vec3){.x = 0.0f, .y = 195.0f, .z = 0.0f},
            .scale = (Vec3){.x = 2.0f, .y = 1.0f, .z = 2.0f},
        }
    };

    for (size_t i = 0; i < 2; i++) {
        instances[i].matrixTransform = init_matrix();
        update_instance_transforms(&instances[i]);
    }

    Scene scene = (Scene) {
        .instances = instances,
        .objectCount = 2
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

        move_cube(&instances[1]);
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

        // TODO: Use mouse to look around
        // Vector2 mouse = GetMouseDelta();
        // printf("mouse x:%+03.02f y:%+03.02f\n",mouse.x, mouse.y);

        if (IsWindowResized()) {
            camera.height = GetScreenHeight();
            camera.width = GetScreenWidth();
            
            camera.canvas = realloc(camera.canvas, sizeof(Color) * camera.height * camera.width);
            if (camera.canvas == NULL) {
                goto closeWindow;
            }

            camera.view.width = (float)camera.width / (float)camera.height;

            ImageResize(&img, camera.width, camera.height);
            UnloadTexture(renderTexture);            
            renderTexture = LoadTextureFromImage(img);
      
        }

        render_scene(camera, scene);
        UpdateTexture(renderTexture, camera.canvas);
        
        BeginDrawing();
            ClearBackground(RAYWHITE);

            DrawTexture(renderTexture, 0, 0, RAYWHITE);
            DrawFPS(15,15);
        EndDrawing();
        // break;
    }

closeWindow:
    for (size_t i = 0; i < scene.objectCount; i++) {
        free(scene.instances[i].matrixTransform);
    }
    free_model(cube);
    free(camera.canvas);
    free(camera.matrixTransform);
    UnloadImage(img);
    UnloadTexture(renderTexture);

    CloseWindow();    
    return 0;
}