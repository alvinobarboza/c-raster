#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>

#include "models.h"
#include "shapes.h"
#include "camera.h"
#include "transforms.h"


#define w 60
#define h 30

char canvas[w*h];

void canvas_clear() {
    for (int i = 0; i < w*h; i++) {
        canvas[i] = '.';
    }
}

void render_canvas() {
    for(int y = 0; y < h; y++){
        for(int x = 0; x < w; x++) {
            printf("%c", canvas[y*w+x]);
        }
        puts(" ");
    }
}

void draw_top_bottom(Point pointA, Point pointB, Point pointC) {
    if (pointB.x > pointC.x) {
        Point t = pointB;
        pointB = pointC;
        pointC = t;
    }

    float invslope1 = (float)(pointB.x - pointA.x) / (float)(pointB.y - pointA.y);
    float invslope2 = (float)(pointC.x - pointA.x) / (float)(pointC.y - pointA.y);

    float curx1 = pointA.x;
    float curx2 = pointA.x;

    for(int scanlineY = pointA.y; scanlineY <= pointC.y; scanlineY++) {

        for(int x = curx1; x <= curx2; x++){
            int i = scanlineY*w+x;
            if (i >= 0 && i < w*h ){
                canvas[i] = '0';
            }
        }

        curx1 += invslope1;
        curx2 += invslope2;
    }
}

void draw_bottom_top(Point pointA, Point pointB, Point pointC) {
    if (pointB.x < pointA.x) {
        Point t = pointB;
        pointB = pointA;
        pointA = t;
    }

    float invslope1 = (float)(pointC.x - pointA.x) / (float)(pointC.y - pointA.y);
    float invslope2 = (float)(pointC.x - pointB.x) / (float)(pointC.y - pointB.y);

    float curx1 = pointC.x;
    float curx2 = pointC.x;

    for(int scanlineY = pointC.y; scanlineY > pointA.y; scanlineY--) {

        for(int x = curx1; x <= curx2; x++){
            int i = scanlineY*w+x;
            if (i >= 0 && i < w*h){
                canvas[i] = '0';
            }
        }

        curx1 -= invslope1;
        curx2 -= invslope2;
    }
}

int main() {

    // Start measuring time
    struct timeval begin, end;
    gettimeofday(&begin, 0);

    // CODE 
    // testing new algorithm to avoid so many mallocs and make it easier to interpolate
    // between normals and uvs 
    // Test subject: https://www.sunshine2k.de/coding/java/TriangleRasterization/TriangleRasterization.html

    canvas_clear();

    Point pointA = (Point) {.x = 50, .y = 16};
    Point pointB = (Point) {.x = 5, .y = 5};
    Point pointC = (Point) {.x = 20, .y = 27};

    if (pointB.y < pointA.y) {
        Point t = pointB;
        pointB = pointA;
        pointA = t;
    }

    if (pointC.y < pointA.y) {
        Point t = pointC;
        pointC = pointA;
        pointA = t;
    }

    if (pointC.y < pointB.y) {
        Point t = pointC;
        pointC = pointB;
        pointB = t;
    }
    
    if (pointB.y == pointC.y) {
        draw_top_bottom(pointA, pointB, pointC);
    } else if (pointA.y == pointB.y) {
        draw_bottom_top(pointA, pointB, pointC);
    } else {
        Point pointAC = (Point) {
            .x = pointA.x + ((float)(pointB.y - pointA.y) / (float)(pointC.y - pointA.y) * (float)(pointC.x - pointA.x)), 
            .y = pointB.y};
        draw_top_bottom(pointA, pointB, pointAC);
        draw_bottom_top(pointB, pointAC, pointC);        
    }

    canvas[pointA.y*w+pointA.x] = 'A';
    canvas[pointB.y*w+pointB.x] = 'B';
    canvas[pointC.y*w+pointC.x] = 'C';

    render_canvas();

    int count = 100;
    int data = count * 0.5;

    printf("%d\n", data);

    // END CODE


    // Stop measuring time and calculate the elapsed time
    gettimeofday(&end, 0);
    long seconds = end.tv_sec - begin.tv_sec;
    long microseconds = end.tv_usec - begin.tv_usec;
    double elapsed = seconds + microseconds*1e-6;
    
    printf("Time measured: %.5f seconds.\n", elapsed);
	return 0;
}