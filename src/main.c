#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "raylib.h"

#define WIDTH  1920
#define HEIGHT 1080
#define OPAQUE 255
#define TARGET_FPS 30

typedef struct Vec2 {
    int x;
    int y;
} Vec2;

void swap_vec_values(Vec2 *p1, Vec2 *p2) {
    Vec2 temp = *p1;
    *p1 = *p2;
    *p2 = temp;
}

typedef struct Canvas {
    Color *pixels;
    int width;
    int height;
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
    x = c.width/2 + x;
	y = c.height/2 - y - 1;

	if (x < 0 || x >= c.width || y < 0 || y >= c.height) {
		return;
	}

	c.pixels[y*c.width+x] = color;
}

// Must deallocate after use
float * interpolate(int i0, int d0, int i1, int d1) {
    float *values;

    if (i0 == i1) {
        values = malloc(sizeof(float) * 1);
        values[0] = d0;
        return values;
    }
    
    int di = i1 - i0;
    int dd = d1 - d0;

    values = malloc(sizeof(float) * abs(di) + 1);

    float a = (float)(dd) / (float)(di);
    float d = d0;

    for (int i = i0; i <= i1; i++) {
        // printf("d=%f index-d=%d size=%d\n", d , i-i0 , di );
        values[i-i0] = d;
        d = d + a;
    }

    return values;
}

void draw_line(Canvas c, Vec2 pos0, Vec2 pos1 ){
    int dx = pos1.x - pos0.x;
    int dy = pos1.y - pos0.y;
    // printf("--------dx: %d dy: %d---------\n", dx, dy);

    if ( abs(dx) > abs(dy) ) {
        if ( dx < 0 ) {
            swap_vec_values(&pos0, &pos1);
        }

        float *ys = interpolate(pos0.x, pos0.y, pos1.x, pos1.y);

        for( int x = pos0.x; x <= pos1.x; x++) {
            // printf("dx > dy - x=%d y=%f index-y=%d\n",x, ys[x - pos0.x], x - pos0.x);
    
            put_pixel( c, BLACK, x, ys[x - pos0.x] );
        }

        free(ys);
        return;
    }

    if ( dy < 0 ) {
        swap_vec_values(&pos0, &pos1);
    }

    float *xs = interpolate(pos0.y, pos0.x, pos1.y, pos1.x);

    for( int y = pos0.y; y <= pos1.y; y++) {
        // printf("dx < dy - xs[%d]:%f y:%d\n", y - pos0.y, xs[y - pos0.y], y);
        
        put_pixel( c, BLACK, xs[y - pos0.y], y );
    }

    free(xs);
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

        Vec2 point1 = {0};
        Vec2 point2 = {0};

        point2.x = (cos(GetTime()) * (canvas.width / 2 / 2)) ;
        point2.y = (sin(GetTime()) * (canvas.height / 2 / 2));
        draw_line( canvas, point1, point2 );

        point1.x = -50;
        point1.y = -200;

        point2.x = 60;
        point2.y = 240;
        draw_line( canvas, point1, point2 );

        point1.x = -200;
        point1.y = -100;

        point2.x = 240;
        point2.y = 120;
        draw_line( canvas, point1, point2 );

        UpdateTexture(renderTexture, canvas.pixels);

        
        BeginDrawing();
            ClearBackground(RAYWHITE);

            DrawTexture(renderTexture, 0, 0, RAYWHITE);
            DrawText("Hello from raylib!", 190, 200, 20, LIGHTGRAY);
            DrawFPS(15,15);
        EndDrawing();
        // break;
    }

closeWindow:      
    free(canvas.pixels);
    UnloadImage(img);
    UnloadTexture(renderTexture);

    CloseWindow();    
    return 0;
}