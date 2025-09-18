#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "raylib.h"
#include "transforms.h"
#include "camera.h"
#include "models.h"
#include "render.h"

#define WIDTH  1080
#define HEIGHT 1080
#define TARGET_FPS 60

// Cube Data =========
Vec3 verts[]  = {
    { 1.0f,  1.0f,  1.0f},
    {-1.0f,  1.0f,  1.0f},
    {-1.0f, -1.0f,  1.0f},
    { 1.0f, -1.0f,  1.0f},
    { 1.0f,  1.0f, -1.0f},
    {-1.0f,  1.0f, -1.0f},
    {-1.0f, -1.0f, -1.0f},
    { 1.0f, -1.0f, -1.0f},
};

Triangle tris[] = {
    {.v1 = 0, .v2 = 1, .v3 = 2, .color = RED},
    {.v1 = 0, .v2 = 2, .v3 = 3, .color = RED},
    {.v1 = 4, .v2 = 0, .v3 = 3, .color = GREEN},
    {.v1 = 4, .v2 = 3, .v3 = 7, .color = GREEN},
    {.v1 = 5, .v2 = 4, .v3 = 7, .color = BLUE},
    {.v1 = 5, .v2 = 7, .v3 = 6, .color = BLUE},
    {.v1 = 1, .v2 = 5, .v3 = 6, .color = YELLOW},
    {.v1 = 1, .v2 = 6, .v3 = 2, .color = YELLOW},
    {.v1 = 4, .v2 = 5, .v3 = 1, .color = PURPLE},
    {.v1 = 4, .v2 = 1, .v3 = 0, .color = PURPLE},
    {.v1 = 2, .v2 = 6, .v3 = 7, .color = (Color) {.a = OPAQUE, .r = 0, .g = OPAQUE, .b = OPAQUE} },
    {.v1 = 2, .v2 = 7, .v3 = 3, .color = (Color) {.a = OPAQUE, .r = 0, .g = OPAQUE, .b = OPAQUE} },
};

// Cube Data ========

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

    ModelData cube = (ModelData) {
        .tris = tris,
        .verts = verts,
        .trisCount = 12,
        .vertsCount = 8,
    };

    Instance instances[2] = {
        (Instance) {
            .model = &cube,
            .position = (Vec3){.x = -1.5, .y = 0.0f, .z = 7.0f},
            .rotation = (Vec3){.x = 0.0f, .y = 0.0f, .z = 0.0f},
            .scale = (Vec3){.x = .75f, .y = .75f, .z = .75f},
        },
        (Instance) {
            .model = &cube,
            .position = (Vec3){.x = 1.25, .y = 2.5f, .z = 7.5f},
            .rotation = (Vec3){.x = 0.0f, .y = 195.0f, .z = 0.0f},
            .scale = (Vec3){.x = 1.0f, .y = 1.0f, .z = 1.0f},
        }
    };

    for (size_t i = 0; i < 2; i++) {
        instances[i].matrixTransform = init_matrix();
        update_instance_transforms(&instances[i]);
    }
    
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

        for(size_t i = 0; i < 2; i++){
            render_model(camera, instances[i]);
        }

        UpdateTexture(renderTexture, camera.canvas);

        
        BeginDrawing();
            ClearBackground(RAYWHITE);

            DrawTexture(renderTexture, 0, 0, RAYWHITE);
            DrawFPS(15,15);
        EndDrawing();
        // break;
    }

closeWindow:
    for (size_t i = 0; i < 2; i++)
    {
        free(instances[i].matrixTransform);
    }
    
    free(camera.canvas);
    free(camera.matrixTransform);
    UnloadImage(img);
    UnloadTexture(renderTexture);

    CloseWindow();    
    return 0;
}