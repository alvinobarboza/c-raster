#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "raylib.h"
#include "transforms.h"
#include "camera.h"
#include "models.h"
#include "render.h"
#include "scene.h"
#include "shapes.h"

#define WIDTH  1920
#define HEIGHT 800
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
        instance->transforms.rotation.y += .5f;
        update_instance_transforms(instance);
    }
    
    if (IsKeyDown(KEY_O)) {
        instance->transforms.rotation.y -= .5f;
        update_instance_transforms(instance);
    }
}

int main(void)
{
    float moveSpeed = 15.0f;
    float turnSpeed = 70.0f;

    Cam camera = init_camera(
        WIDTH, HEIGHT, 
        (Vec3) {.x = -.87f, .y = 4.10f, .z = 9.94f},
        (Vec3) {.x = -22.17f, .y = -103.54f, .z = 0.0f}
    );

    // TODO: proper model loading
    ModelData cube = cube_shape();
    ModelData icosahedron = icosahedron_shape();
    ModelData torus = torus_shape();
    ModelData uvSphere = uv_sphere_shape();
    ModelData square = square_shape();

    // TODO: better instance loading
    Instance instances[] = {
        init_instance(&square, (Transforms){
            .position = (Vec3){.x = 0.0f, .y = 0.0f, .z = 7.0f},
            .rotation = (Vec3){.x = 0.0f, .y = 0.0f, .z = 30.0f},
            .scale = (Vec3){.x = 1.0f, .y = 1.0f, .z = 1.0f}
        }),
        init_instance(&cube, (Transforms) {
            .position = (Vec3){.x = -2.5, .y = 0.0f, .z = 10.0f},
            .rotation = (Vec3){.x = 0.0f, .y = 0.0f, .z = 0.0f},
            .scale = (Vec3){.x = .7f, .y = .7f, .z = .7f},
        }),
        init_instance(&cube, (Transforms) {
            .position = (Vec3){.x = 2.25, .y = 2.5f, .z = 9.5f},
            .rotation = (Vec3){.x = 0.0f, .y = 195.0f, .z = 0.0f},
            .scale = (Vec3){.x = 2.0f, .y = 2.0f, .z = 2.0f},
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
        .objectCount = 6
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

            camera.view.width = ((float)camera.width / (float)camera.height)* 2.5f;

            ImageResize(&img, camera.width, camera.height);
            UnloadTexture(renderTexture);            
            renderTexture = LoadTextureFromImage(img);
      
        }

        render_scene(camera, scene);
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
    free(camera.canvas);
    free(camera.matrixTransform);
    UnloadImage(img);
    UnloadTexture(renderTexture);

    CloseWindow();    
    return 0;
}