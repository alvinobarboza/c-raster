#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "raylib.h"
#include "vector.h"
#include "camera.h"
#include "model3d.h"
#include "drawing.h"

#define WIDTH  1080
#define HEIGHT 1080
#define TARGET_FPS 30

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
    {.v1 = 2, .v2 = 6, .v3 = 7, .color = MAGENTA},
    {.v1 = 2, .v2 = 7, .v3 = 3, .color = MAGENTA},
};

// Cube Data ========

int main(void)
{
    Cam canvas = (Cam) {
        .canvas = malloc(sizeof(Color) * WIDTH * HEIGHT),
        .width = WIDTH,
        .height = HEIGHT,
        .view = (Viewport) {.d = 1.0f, .height = 1.0f, .width = 1.0f}
    };

    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_ALWAYS_RUN | FLAG_MSAA_4X_HINT);
    InitWindow(canvas.width, canvas.height, "raylib [core] example - basic window");

    Image img = GenImageColor(canvas.width, canvas.height, RAYWHITE);
    Texture2D renderTexture = LoadTextureFromImage(img);
    
    SetTargetFPS(TARGET_FPS);

    ModelData cube = (ModelData) {
        .tris = tris,
        .verts = verts,
        .trisCount = 12,
        .vertsCount = 8,
    };

    Instance instance = (Instance) {
        .model = &cube,
        .rotation = (Vec3){0},
        .scale = (Vec3){.x = 1.0f, .y = 1.0f, .z = 1.0f},
        .transform = (Vec3){.x = -1.5, .y = 0.0f, .z = 7.0f},
    };

    while (!WindowShouldClose())
    {
        clear_canvas(canvas);

        if (IsWindowResized()) {
            canvas.height = GetScreenHeight();
            canvas.width = GetScreenWidth();
            
            canvas.canvas = realloc(canvas.canvas, sizeof(Color) * canvas.height * canvas.width);
            if (canvas.canvas == NULL) {
                goto closeWindow;
            }

            ImageResize(&img, canvas.width, canvas.height);
            UnloadTexture(renderTexture);            
            renderTexture = LoadTextureFromImage(img);
      
        }

        render_model(canvas, instance);

        UpdateTexture(renderTexture, canvas.canvas);

        
        BeginDrawing();
            ClearBackground(RAYWHITE);

            DrawTexture(renderTexture, 0, 0, RAYWHITE);
            DrawText("Hello from raylib!", 190, 200, 20, LIGHTGRAY);
            DrawFPS(15,15);
        EndDrawing();
        // break;
    }

closeWindow:      
    free(canvas.canvas);
    UnloadImage(img);
    UnloadTexture(renderTexture);

    CloseWindow();    
    return 0;
}