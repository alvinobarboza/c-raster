#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "raylib.h"

#define WIDTH  800
#define HEIGHT 450
#define OPAQUE 255
#define TARGET_FPS 60

typedef struct Vec2 {
    int x;
    int y;
} Vec2;

typedef struct Canvas {
    Color *pixels;
    size_t width;
    size_t height;
} Canvas;

void clear_canvas(Canvas c){
    size_t length = c.height * c.width;
    for( size_t i = 0; i < length; i++) {
        c.pixels[i].r = OPAQUE;
        c.pixels[i].g = OPAQUE;
        c.pixels[i].b = OPAQUE;
        c.pixels[i].a = OPAQUE;
    }
}

void put_pixel(Canvas c, Color color, int x, int y) {
    size_t i = y*c.width+x;
    if ( i >= c.height * c.width ) {
        return;
    }
    c.pixels[i] = color;
}

void draw_line(Canvas c, Vec2 pos0, Vec2 pos1 ){
    if (pos0.x > pos1.x) {
        Vec2 temp = pos0;
        pos0 = pos1;
        pos1 = temp;
    }

    float a = (float)(pos1.y - pos0.y) / (float) (pos1.x - pos0.x);
    float y = pos0.y;

    for( int x = pos0.x; x <= pos1.x; x++) {
        put_pixel(c, BLACK, x, y);
        y = y + a;
    }
}

int main(void)
{
    Canvas canvas = (Canvas) {
        .pixels = malloc(sizeof(Color) * WIDTH * HEIGHT),
        .width = WIDTH,
        .height = HEIGHT,
    };

    SetConfigFlags( FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_ALWAYS_RUN | FLAG_MSAA_4X_HINT);
    InitWindow(canvas.width, canvas.height, "raylib [core] example - basic window");

    Image img = GenImageColor(canvas.width, canvas.height, RAYWHITE);
    Texture2D renderTexture = LoadTextureFromImage(img);
    
    SetTargetFPS(TARGET_FPS);

    while (!WindowShouldClose())
    {
        clear_canvas(canvas);

        if (IsWindowResized()) {
            canvas.height = GetScreenHeight();
            canvas.width = GetScreenWidth();
            
            canvas.pixels = realloc(canvas.pixels, sizeof(Color) * canvas.height * canvas.width);
            if (canvas.pixels == NULL) {
                goto closeWindow;
            }

            ImageResize(&img, canvas.width, canvas.height);
            UnloadTexture(renderTexture);            
            renderTexture = LoadTextureFromImage(img);
      
        }

        Vec2 point1 = (Vec2){
            .x = (cos(GetTime()) * (canvas.width / 2)) + canvas.width / 2, 
            .y = (sin(GetTime()) * (canvas.height / 2)) + canvas.height / 2 
        };

        Vec2 point2 = (Vec2){
            .x = canvas.width / 2, 
            .y = canvas.height / 2 
        };

        draw_line( canvas, point1, point2 );
        UpdateTexture(renderTexture, canvas.pixels);

        
        BeginDrawing();
            ClearBackground(RAYWHITE);

            DrawTexture(renderTexture, 0, 0, RAYWHITE);
            DrawText("Hello from raylib!", 190, 200, 20, LIGHTGRAY);
            DrawFPS(15,15);
        EndDrawing();
    }

closeWindow:  
    CloseWindow();

    free(canvas.pixels);
    UnloadImage(img);
    UnloadTexture(renderTexture);

    return 0;
}