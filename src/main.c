#include <stdlib.h>
#include <stdio.h>

#include "raylib.h"

#define WIDTH  800
#define HEIGHT 450
#define OPAQUE 255
#define TARGET_FPS 60

void draw_square(Color *canvas, size_t w , size_t h) {
    int canvas_size = w*h;

    for (size_t y = 0; y < h; y++) {
        for (size_t x = 0; x < w; x++)
        {
            if (x < w/2/2 || w/2+w/2/2 < x) {
                canvas[y*w+x].g = 0;
                canvas[y*w+x].r = 0;
                canvas[y*w+x].b = 0;
                canvas[y*w+x].a = OPAQUE;
                continue; 
            }
            if (y < h/2/2 || h/2+h/2/2 < y) {
                canvas[y*w+x].g = 0;
                canvas[y*w+x].r = 0;
                canvas[y*w+x].b = 0;
                canvas[y*w+x].a = OPAQUE;
                continue; 
            }
            canvas[y*w+x].g = (((y*100/h)) * 255) / 100;
            canvas[y*w+x].r = (((x*100/w)) * 255) / 100;
            canvas[y*w+x].b = (((y*x*100/canvas_size)) * 255) / 100;
            canvas[y*w+x].a = OPAQUE;
        }   
    }
}

int main(void)
{
    int currentScreenWidth = WIDTH;
    int currentScreenHeight = HEIGHT;

    SetConfigFlags( FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_ALWAYS_RUN | FLAG_MSAA_4X_HINT);
    InitWindow(currentScreenWidth, currentScreenHeight, "raylib [core] example - basic window");

    Image img = GenImageColor(currentScreenWidth, currentScreenHeight, RAYWHITE);
    Texture2D renderTexture = LoadTextureFromImage(img);
    
    int canvas_size = currentScreenWidth * currentScreenHeight;
    Color *canvas = malloc(sizeof(Color) * canvas_size);
    
    draw_square(canvas, currentScreenWidth, currentScreenHeight);
    UpdateTexture(renderTexture, canvas);
    
    SetTargetFPS(TARGET_FPS);
    while (!WindowShouldClose())
    {
        if (IsWindowResized()) {
            currentScreenHeight = GetScreenHeight();
            currentScreenWidth = GetScreenWidth();
            canvas_size = currentScreenHeight * currentScreenWidth;
            
            canvas = realloc(canvas, sizeof(Color) * canvas_size);
            if (canvas == NULL) {
                goto closeWindow;
            }

            ImageResize(&img, currentScreenWidth, currentScreenHeight);
            UnloadTexture(renderTexture);            
            renderTexture = LoadTextureFromImage(img);
        }

        draw_square(canvas, currentScreenWidth, currentScreenHeight);
        UpdateTexture(renderTexture, canvas);
        
        BeginDrawing();
            ClearBackground(RAYWHITE);

            DrawTexture(renderTexture, 0, 0, RAYWHITE);
            DrawText("Hello from raylib!", 190, 200, 20, LIGHTGRAY);
            DrawFPS(15,15);
        EndDrawing();
    }

closeWindow:  
    CloseWindow();

    free(canvas);
    UnloadImage(img);
    UnloadTexture(renderTexture);

    return 0;
}