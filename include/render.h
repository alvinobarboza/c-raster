#ifndef _RENDER_H
#define _RENDER_H

#include "camera.h"
#include "models.h"
#include "scene.h"

void draw_line(Cam c, Point pos0, Point pos1 , Color color);
void draw_wireframe_triangle(Cam c, Point p0, Point p1, Point p2, Color color);
void draw_filled_triangle(Cam c, Point p0, Point p1, Point p2, Color color);
void render_triangle(Cam c, FullTriangle tri);
void render_model(Cam c, Instance instance);
void render_scene(Cam c, Scene scene);

#endif