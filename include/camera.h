#ifndef _CAMERA_H
#define _CAMERA_H

#include "raylib.h"
#include "transforms.h"

#define OPAQUE 255
#define DEFAULT_FOV 52.9 // TODO: make change here reflect View D or render FOV
#define VIEW_PLANE_DISTANCE 1.0f
#define FRUSTUM_PLANES 6
#define NEAR_PLANE_DISTANCE .2f
#define FAR_PLANE_DISTANCE 40.f

// To be honest, this is actually aspect ratio related
typedef struct Viewport {
    float width, height, d;
} Viewport;

// View planes for clipping objects out
typedef struct ViewPlane {
    Vec3 normal;
    float d;
} ViewPlane;

// Camera struct holding all render related properties
// "Cam" to avoid conflict with raylib "Camera"
typedef struct Cam {
    Color *canvas;
    float *depthBuffer;
    float *matrixTransform;
    Viewport view;
    int width;
    int height;
    Vec3 scale;
    Vec3 rotation;
    Vec3 position;
    Vec3 forwardDirection;
    ViewPlane frustum[FRUSTUM_PLANES];
} Cam;

// Point in the screen(screen coord), whole number (int)
typedef struct Point {
    int x, y;
    float brightness, zDepth;
} Point;

void swap_point_values(Point *p1, Point *p2);
void put_pixel(Cam c, Color color, int x, int y, float h, float depth);
void clear_canvas(Cam c);

Point viewport_to_canvas(Cam c, float x, float y, float z);
Point project_vertex(Cam c, Vec3 v);

Cam init_camera(int w, int h, Vec3 position, Vec3 rotation);
void free_camera(Cam c);

void update_camera_transforms(Cam *c);
void update_camera_frustum(Cam *c, int w, int h);

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