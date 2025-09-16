#include <stddef.h>
#include "camera.h"

void swap_point_values(Point *p1, Point *p2) {
    Point temp = *p1;
    *p1 = *p2;
    *p2 = temp;
}

void put_pixel(Cam c, Color color, int x, int y) {
    x = c.width/2 + x;
	y = c.height/2 - y - 1;

	if (x < 0 || x >= c.width || y < 0 || y >= c.height) {
		return;
	}

	c.canvas[y*c.width+x] = color;
}

void clear_canvas(Cam c) { 
    size_t length = c.height * c.width;
    for(size_t i = 0; i < length; i++) {
        c.canvas[i].r = OPAQUE;
        c.canvas[i].g = OPAQUE;
        c.canvas[i].b = OPAQUE;
        c.canvas[i].a = OPAQUE;
    }
}

Point viewport_to_canvas(Cam c, float x, float y) {
    return (Point) {
        .brightness = 1.0f,
        .x = x * c.width/c.view.width,
        .y = y * c.height/c.view.height
    };
}

Point project_vertex(Cam c, Vec3 v) {
    return viewport_to_canvas(
        c, 
        v.x * c.view.d / v.z,
        v.y * c.view.d / v.z
    );
}