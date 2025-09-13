#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "raylib.h"

#define WIDTH  1080
#define HEIGHT 1080
#define OPAQUE 255
#define TARGET_FPS 30


typedef struct Viewport {
    float width, height, d;
} Viewport;

typedef struct Canvas {
    Color *pixels;
    int width;
    int height;
    Viewport view;
} Canvas;

typedef struct Point {
    int x, y;
    float h;
} Point;

typedef struct Vertex
{
    float x, y, z;
} Vertex;

void swap_vec_values(Point *p1, Point *p2) {
    Point temp = *p1;
    *p1 = *p2;
    *p2 = temp;
}

Point viewport_to_canvas(Canvas c, float x, float y) {
    return (Point) {
        .h = 1.0f,
        .x = x * c.width/c.view.width,
        .y = y * c.height/c.view.height
    };
}

Point project_vertex(Canvas c, Vertex v) {
    return viewport_to_canvas(
        c, 
        v.x * c.view.d / v.z,
        v.y * c.view.d / v.z
    );
}

void clear_canvas(Canvas c){
    size_t length = c.height * c.width;
    for(size_t i = 0; i < length; i++) {
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
float * interpolate(int i0, float d0, int i1, float d1) {
    float *values;

    if (i0 == i1) {
        values = malloc(sizeof(float) * 1);
        values[0] = d0;
        return values;
    }
    
    int di = i1 - i0;
    float dd = d1 - d0;

    // Last value included
    values = malloc(sizeof(float) * abs(di) + 1);

    float a = dd / (float)(di);
    float d = d0;

    for (int i = i0; i <= i1; i++) {
        // printf("d=%f index-d=%d size=%d\n", d , i-i0 , di);
        values[i-i0] = d;
        d = d + a;
    }

    return values;
}

void draw_line(Canvas c, Point pos0, Point pos1 , Color color){
    int dx = pos1.x - pos0.x;
    int dy = pos1.y - pos0.y;
    // printf("--------dx: %d dy: %d---------\n", dx, dy);

    if (abs(dx) > abs(dy)) {
        if (dx < 0) {
            swap_vec_values(&pos0, &pos1);
        }

        float *ys = interpolate(pos0.x, pos0.y, pos1.x, pos1.y);

        for(int x = pos0.x; x <= pos1.x; x++) {
            // printf("dx > dy - x=%d y=%f index-y=%d\n",x, ys[x - pos0.x], x - pos0.x);
    
            put_pixel(c, color, x, ys[x - pos0.x]);
        }

        free(ys);
        return;
    }

    if (dy < 0) {
        swap_vec_values(&pos0, &pos1);
    }

    float *xs = interpolate(pos0.y, pos0.x, pos1.y, pos1.x);

    for(int y = pos0.y; y <= pos1.y; y++) {
        // printf("dx < dy - xs[%d]:%f y:%d\n", y - pos0.y, xs[y - pos0.y], y);
        
        put_pixel(c, color, xs[y - pos0.y], y);
    }

    free(xs);
}

void draw_wireframe_triangle(Canvas c, Point p0, Point p1, Point p2, Color color) {
    draw_line(c, p0, p1, color);
    draw_line(c, p1, p2, color);
    draw_line(c, p2, p0, color);
}

void draw_filled_triangle(Canvas c, Point p0, Point p1, Point p2, Color color) {
    // Sort the points so that p0.y <= p1.y <= p2.y
    if (p1.y < p0.y) swap_vec_values(&p1, &p0);
    if (p2.y < p0.y) swap_vec_values(&p2, &p0);
    if (p2.y < p1.y) swap_vec_values(&p2, &p1);

    // Compute the x coordinates and h values of the triangle edges
    float *x01 = interpolate(p0.y, p0.x, p1.y, p1.x); // values from p0.x to p1.x included
    float *h01 = interpolate(p0.y, p0.h, p1.y, p1.h); // values from p0.h to p1.h included
    
    float *x12 = interpolate(p1.y, p1.x, p2.y, p2.x); // values from p1.x to p2.x included 
    float *h12 = interpolate(p1.y, p1.h, p2.y, p2.h); // values from p1.h to p2.h included
    
    float *x02 = interpolate(p0.y, p0.x, p2.y, p2.x); // values from p0.x to p2.x included
    float *h02 = interpolate(p0.y, p0.h, p2.y, p2.h); // values from p0.h to p2.h included

    // => Concatenate the short sides
    // remove_last(x01) -> length trick below
    // remove_last(h01)
    size_t len_x01 = abs(p1.y - p0.y); // last value excluded
    size_t len_x12 = abs(p2.y - p1.y) + 1; // last value included
    size_t len_x012 = len_x01 + len_x12;
    
    // x012 = x01 + x12
    float *x012 = malloc(sizeof(float) * len_x012);
    memcpy(x012, x01, sizeof(float) * len_x01);
    memcpy(x012+len_x01, x12, sizeof(float) * len_x12);

    // h012 = h01 + h12
    float *h012 = malloc(sizeof(float) * len_x012);
    memcpy(h012, h01, sizeof(float) * len_x01);
    memcpy(h012+len_x01, h12, sizeof(float) * len_x12);

    float *x_left = NULL;
    float *h_left = NULL;
    
    float *x_right = NULL;
    float *h_right = NULL;

    // => Determine which is left and which is right
    size_t m = len_x012 / 2;
    if (x02[m] < x012[m]) {
        x_left = x02;
        h_left = h02;

        x_right = x012;
        h_right = h012;
    } else {
        x_left = x012;
        h_left = h012;
        
        x_right = x02;
        h_right = h02;
    }

    for (int y = p0.y; y <= p2.y; y++){
        float x_l = x_left[y-p0.y];
        float x_r = x_right[y-p0.y];

        float *h_segment = interpolate(x_l, h_left[y-p0.y], x_r, h_right[y - p0.y]);

        for(int x = x_l; x <= x_r; x++){
            size_t index = x - x_l;
            float h = h_segment[index];
            // printf(
            //     "r:%d -> %d g:%d -> %d b:%d -> %d h:%f\n", 
            //     color.r, (unsigned char)(color.r*h),
            //     color.g, (unsigned char)(color.g*h),
            //     color.b, (unsigned char)(color.b*h), 
            //     h);

            Color shaded = {0};
            shaded.r = color.r * h;
            shaded.g = color.g * h;
            shaded.b = color.b * h;
            shaded.a = OPAQUE;
            put_pixel(c, shaded, x, y);
        }

        free(h_segment);
    }

    free(x01);
    free(x12);
    free(x02);
    free(h01);
    free(h12);
    free(h02);
    free(x012);
    free(h012);
}

int main(void)
{
    Canvas canvas = (Canvas) {
        .pixels = malloc(sizeof(Color) * WIDTH * HEIGHT),
        .width = WIDTH,
        .height = HEIGHT,
        .view = (Viewport) {.d = 1.0f, .height = 1.0f, .width = 1.0f}
    };

    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_ALWAYS_RUN | FLAG_MSAA_4X_HINT);
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

        // The four "front" vertices
        Vertex vAf = (Vertex){.x = -2,.y = -0.5,.z = 5};
        Vertex vBf = (Vertex){.x = -2,.y =  0.5,.z = 5};
        Vertex vCf = (Vertex){.x = -1,.y =  0.5,.z = 5};
        Vertex vDf = (Vertex){.x = -1,.y = -0.5,.z = 5};

        // The four "back" vertices
        Vertex vAb = (Vertex){.x = -2,.y = -0.5,.z = 6};
        Vertex vBb = (Vertex){.x = -2,.y =  0.5,.z = 6};
        Vertex vCb = (Vertex){.x = -1,.y =  0.5,.z = 6};
        Vertex vDb = (Vertex){.x = -1,.y = -0.5,.z = 6};

        // The front face
        draw_line(canvas ,project_vertex(canvas, vAf), project_vertex(canvas, vBf), BLUE);
        draw_line(canvas ,project_vertex(canvas, vBf), project_vertex(canvas, vCf), BLUE);
        draw_line(canvas ,project_vertex(canvas, vCf), project_vertex(canvas, vDf), BLUE);
        draw_line(canvas ,project_vertex(canvas, vDf), project_vertex(canvas, vAf), BLUE);

        // The back face
        draw_line(canvas ,project_vertex(canvas, vAb), project_vertex(canvas, vBb), RED);
        draw_line(canvas ,project_vertex(canvas, vBb), project_vertex(canvas, vCb), RED);
        draw_line(canvas ,project_vertex(canvas, vCb), project_vertex(canvas, vDb), RED);
        draw_line(canvas ,project_vertex(canvas, vDb), project_vertex(canvas, vAb), RED);

        // The front-to-back edges
        draw_line(canvas ,project_vertex(canvas, vAf), project_vertex(canvas, vAb), GREEN);
        draw_line(canvas ,project_vertex(canvas, vBf), project_vertex(canvas, vBb), GREEN);
        draw_line(canvas ,project_vertex(canvas, vCf), project_vertex(canvas, vCb), GREEN);
        draw_line(canvas ,project_vertex(canvas, vDf), project_vertex(canvas, vDb), GREEN);

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