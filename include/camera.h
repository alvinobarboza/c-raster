#ifndef _CAMERA_H
#define _CAMERA_H

#include "raylib.h"
#include "transforms.h"

#define OPAQUE 255

// To be honest, this is actually aspect ratio related
typedef struct Viewport {
    float width, height, d;
} Viewport;

// Camera struct holding all render related properties
// "Cam" to avoid conflict with raylib "Camera"
typedef struct Cam {
    Color *canvas;
    float *matrixTransform;
    Viewport view;
    int width;
    int height;
    Vec3 scale;
    Vec3 rotation;
    Vec3 position;
    Vec3 forwardDirection;
} Cam;

// Point in the screen(screen coord), whole number (int)
typedef struct Point {
    int x, y;
    float brightness;
} Point;

void swap_point_values(Point *p1, Point *p2);
void put_pixel(Cam c, Color color, int x, int y);
void clear_canvas(Cam c);

Point viewport_to_canvas(Cam c, float x, float y);
Point project_vertex(Cam c, Vec3 v);

void update_camera_transforms(Cam *c);

void camera_move_forward(Cam *c, float unit);
void camera_move_backward(Cam *c, float unit);
void camera_move_left(Cam *c, float unit);
void camera_move_right(Cam *c, float unit);
void camera_move_up(Cam *c, float unit);
void camera_move_down(Cam *c, float unit);

void camera_turn_left(Cam *c, float unit);
void camera_turn_right(Cam *c, float unit);
void camera_turn_up(Cam *c, float unit);
void camera_turn_down(Cam *c, float unit);

#endif