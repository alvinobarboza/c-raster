#ifndef _DRAWING_H
#define _DRAWING_H

#include "camera.h"
#include "model3d.h"

void draw_line(Cam c, Point pos0, Point pos1 , Color color);
void draw_wireframe_triangle(Cam c, Point p0, Point p1, Point p2, Color color);
void draw_filled_triangle(Cam c, Point p0, Point p1, Point p2, Color color);
void render_triangle(Cam c, Triangle tri, const Point *projected);
void render_model(Cam c, Instance instance);

#endif