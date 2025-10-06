#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <float.h>
#include "render.h"

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
    values = malloc(sizeof(float) * (abs(di) + 2));

    float a = dd / (float)(di);
    float d = d0;

    for (int i = i0; i <= i1; i++) {
        values[i-i0] = d;
        d = d + a;
    }

    return values;
}

void draw_line(Cam c, Point pos0, Point pos1 , Color color){
    int dx = pos1.x - pos0.x;
    int dy = pos1.y - pos0.y;

    if (abs(dx) > abs(dy)) {
        if (dx < 0) {
            swap_point_values(&pos0, &pos1);
        }

        float *ys = interpolate(pos0.x, pos0.y, pos1.x, pos1.y);
        float *zs = interpolate(pos0.x, pos0.zDepth, pos1.x, pos1.zDepth);

        for(int x = pos0.x; x <= pos1.x; x++) {
            put_pixel(c, color, x, ys[x - pos0.x], 1, zs[x - pos0.x]);
        }

        free(ys);
        free(zs);
        return;
    }

    if (dy < 0) {
        swap_point_values(&pos0, &pos1);
    }

    float *xs = interpolate(pos0.y, pos0.x, pos1.y, pos1.x);
    float *zs = interpolate(pos0.y, pos0.zDepth, pos1.y, pos1.zDepth);

    for(int y = pos0.y; y <= pos1.y; y++) {
        put_pixel(c, color, xs[y - pos0.y], y, 1, zs[y - pos0.y]);
    }

    free(xs);
    free(zs);
}

void draw_wireframe_triangle(Cam c, Point p0, Point p1, Point p2, Color color) {
    draw_line(c, p0, p1, color);
    draw_line(c, p1, p2, color);
    draw_line(c, p2, p0, color);
}

void draw_filled_triangle(Cam c, Point p0, Point p1, Point p2, Color color) {
    // Sort the points so that p0.y <= p1.y <= p2.y
    if (p1.y < p0.y) swap_point_values(&p1, &p0);
    if (p2.y < p0.y) swap_point_values(&p2, &p0);
    if (p2.y < p1.y) swap_point_values(&p2, &p1);

    // Compute the x coordinates and h values of the triangle edges
    float *x01 = interpolate(p0.y, p0.x, p1.y, p1.x); // values from p0.x to p1.x included
    float *h01 = interpolate(p0.y, p0.brightness, p1.y, p1.brightness); // values from p0.brightness to p1.brightness included
    float *z01 = interpolate(p0.y, p0.zDepth, p1.y, p1.zDepth); // values from p0.zDepth to p1.zDepth included
    
    float *x12 = interpolate(p1.y, p1.x, p2.y, p2.x); // values from p1.x to p2.x included 
    float *h12 = interpolate(p1.y, p1.brightness, p2.y, p2.brightness); // values from p1.brightness to p2.brightness included
    float *z12 = interpolate(p1.y, p1.zDepth, p2.y, p2.zDepth); // values from p1.zDepth to p2.zDepth included
    
    float *x02 = interpolate(p0.y, p0.x, p2.y, p2.x); // values from p0.x to p2.x included
    float *h02 = interpolate(p0.y, p0.brightness, p2.y, p2.brightness); // values from p0.brightness to p2.brightness included
    float *z02 = interpolate(p0.y, p0.zDepth, p2.y, p2.zDepth); // values from p0.zDepth to p2.zDepth included

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

    // z012 = z01 + z12
    float *z012 = malloc(sizeof(float) * len_x012);
    memcpy(z012, z01, sizeof(float) * len_x01);
    memcpy(z012+len_x01, z12, sizeof(float) * len_x12);

    float *x_left = NULL;
    float *h_left = NULL;
    float *z_left = NULL;
    
    float *x_right = NULL;
    float *h_right = NULL;
    float *z_right = NULL;

    // => Determine which is left and which is right
    size_t m = len_x012 / 2;
    if (x02[m] < x012[m]) {
        x_left = x02;
        h_left = h02;
        z_left = z02;

        x_right = x012;
        h_right = h012;
        z_right = z012;
    } else {
        x_left = x012;
        h_left = h012;
        z_left = z012;
        
        x_right = x02;
        h_right = h02;
        z_right = z02;
    }

    for (int y = p0.y; y <= p2.y; y++){
        float x_l = x_left[y-p0.y];
        float x_r = x_right[y-p0.y];

        float *h_segment = interpolate(x_l, h_left[y-p0.y], x_r, h_right[y - p0.y]);
        float *z_segment = interpolate(x_l, z_left[y-p0.y], x_r, z_right[y - p0.y]);
        for(int x = x_l; x <= x_r; x++){
            size_t index = x - x_l;
            float h = h_segment[index];
            float depth = z_segment[index];
            put_pixel(c, color, x, y, h, depth);
        }

        free(h_segment);
        free(z_segment);
    }

    free(x01);
    free(x12);
    free(x02);
    free(h01);
    free(h12);
    free(h02);
    free(z01);
    free(z12);
    free(z02);
    free(x012);
    free(h012);
    free(z012);
}

void render_triangle(Cam c, FullTriangle tri) {
    Point pointA = project_vertex(c, tri.vertex[VERTEX_A]);
    Point pointB = project_vertex(c, tri.vertex[VERTEX_B]);
    Point pointC = project_vertex(c, tri.vertex[VERTEX_C]);

    draw_filled_triangle(
        c, 
        pointA,
        pointB,
        pointC, 
        tri.color);
    
    draw_wireframe_triangle(
        c, 
        pointA,
        pointB,
        pointC, 
        BLACK);
}

bool back_face_culling(FullTriangle tri) {
    Vec3 v1 = vec3_sub(tri.vertex[VERTEX_B], tri.vertex[VERTEX_A]);
    Vec3 v2 = vec3_sub(tri.vertex[VERTEX_C], tri.vertex[VERTEX_A]);
    Vec3 n1 = vec3_cross(v1, v2);
    
    float angleA = vec3_by_vec3_multiply(n1, tri.vertex[VERTEX_A]);
    float angleB = vec3_by_vec3_multiply(n1, tri.vertex[VERTEX_B]);
    float angleC = vec3_by_vec3_multiply(n1, tri.vertex[VERTEX_C]);

    return angleA > 0 || angleB > 0 || angleC > 0;
}

// TODO: Get projected point brightness from triangle
void render_model(Cam c, Instance instance) {
    if (instance.trisClippedCount > 0) {
        for (size_t i = 0; i < instance.trisClippedCount; i++){
            render_triangle(c, instance.trisClipped[i]);
        }
    } else {
        for (size_t i = 0; i < instance.model->trisCount; i++) {
            if ( !back_face_culling(instance.trisWorld[i]) ) {
                continue;
            }
            render_triangle(c, instance.trisWorld[i]);
        }
    }
}

float signed_distance_to_point(ViewPlane plane, Vec3 vertex) {
    float d = vec3_by_vec3_multiply(plane.normal, vertex);
    d += plane.d;
    return d;
}

bool is_inside_frustum(ViewPlane *planes, Instance instance) {
    for (unsigned char j = 0; j < FRUSTUM_PLANES; j++) {
        ViewPlane plane = planes[j];

        float d = signed_distance_to_point(plane, instance.boundingSphere.centerWorld);
        float r = instance.boundingSphere.radius;

        if ( d < -r) {
            return false;
        };
    }
    return true;
}

// Formula: 
// t = ( −D − dot(N,A) ) / dot(N,B−A)
// t > Intersection ratio
// D > plane offset value (only used in the front view as it is 1 unit infront of the camera)
// N > Positive normal vector from plane
// A > Vertex in front of the positive plane's normal vector
// B > Vexter behind plane's normal vector 
float ratio_ab_from_plane(ViewPlane plane, Vec3 inFrontA, Vec3 behindB) {
    float normalDotA = vec3_by_vec3_multiply(plane.normal, inFrontA);
    Vec3 bMinusA = vec3_sub(behindB,inFrontA);
    float normalDotBminusA = vec3_by_vec3_multiply(plane.normal, bMinusA);
    float t = (-plane.d - normalDotA) / normalDotBminusA;
    return t;
}

// Formula: 
// Q = A + t(B−A)
// Q > Actual point on plane 
// t > Intersection ratio
// A > Vertex in front of the positive plane's normal vector
// B > Vexter behind plane's normal vector 
Vec3 point_on_plane_from_ab(Vec3 inFrontA, Vec3 behindB, float t) {
    return vec3_add(inFrontA, vec3_multiply(vec3_sub(behindB, inFrontA),t));
}

Vec3 intersection_on_plane(ViewPlane plane, Vec3 inFrontA, Vec3 behindB) {
    float t = ratio_ab_from_plane(plane, inFrontA, behindB);
    Vec3 q = point_on_plane_from_ab(inFrontA, behindB, t);
    return q;
}

void one_vertex_in_front(
    ViewPlane plane, Instance *clipped, size_t indexCurrent,
    Vec3 pA, Vec3 pB, Vec3 pC, 
    FullTriangle t, unsigned char newB, unsigned char newC
) {
    Vec3 pointBprime = intersection_on_plane(plane, pA, pB);
    Vec3 pointCprime = intersection_on_plane(plane, pA, pC);

    t.vertex[newB] = pointBprime;
    t.vertex[newC] = pointCprime;

    clipped->trisClipped[indexCurrent] = t;

    clipped->trisClippedCount++;
}

void two_vertices_in_front(
    ViewPlane plane, Instance *clipped,
    Vec3 pA, Vec3 pB, Vec3 pC, 
    FullTriangle t, unsigned char sharedIndex, unsigned char newIndex,
    size_t indexCurrent, size_t indexAhead
) {
    FullTriangle t1 = t;
    FullTriangle t2 = t;

    Vec3 pointAprime = intersection_on_plane(plane, pA, pC);
    Vec3 pointBprime = intersection_on_plane(plane, pB, pC);
    
    t1.vertex[sharedIndex] = pointAprime;
    t2.vertex[newIndex] = pointAprime;
    t2.vertex[sharedIndex] = pointBprime;

    clipped->trisClipped[indexCurrent] = t1;
    clipped->trisClipped[indexAhead] = t2;

    clipped->trisClippedCount++;
}

void render_scene(Cam c, Scene scene) {
    float m_transform[M4X4];

    for(size_t i = 0; i < scene.objectCount; i++){
        if ( i > 2) {
            continue;
        }
        Instance *clipped = &scene.instances[i];

        matrix_multiplication(c.matrixTransform, clipped->transforms.matrixTransform, m_transform);
        clipped->boundingSphere.centerWorld = mult_matrix_by_vec3(m_transform, clipped->boundingSphere.center);

        if (!is_inside_frustum(c.frustum, *clipped)) {
            continue;
        }

        for (size_t n = 0; n < clipped->model->trisCount; n++) {
            Triangle tp = clipped->model->tris[n];
            
            clipped->trisWorld[n].color = tp.color;
            clipped->trisWorld[n].vertex[VERTEX_A] = mult_matrix_by_vec3(m_transform, clipped->model->verts[tp.v1]);
            clipped->trisWorld[n].vertex[VERTEX_B] = mult_matrix_by_vec3(m_transform, clipped->model->verts[tp.v2]);
            clipped->trisWorld[n].vertex[VERTEX_C] = mult_matrix_by_vec3(m_transform, clipped->model->verts[tp.v3]);
        }

        clipped->trisClippedCount = 0;
        for (size_t j = 0; j < FRUSTUM_PLANES; j++) {
            ViewPlane plane = c.frustum[j];

            float d = signed_distance_to_point(plane, clipped->boundingSphere.centerWorld);
            float r = clipped->boundingSphere.radius;

            if ( d > r ) {
                continue;
            }

            size_t trisLength = clipped->model->trisCount;
            FullTriangle *tris = clipped->trisWorld;

            if (clipped->trisClippedCount > 0){
                trisLength = clipped->trisClippedCount;
                tris = clipped->trisClipped;
            }

            clipped->trisClippedCount = 0;
            size_t endBufferIndex = 0;
            
            for (size_t n = 0; n < trisLength; n++) {
                FullTriangle tri = tris[n];

                if ( !back_face_culling(tri) ){
                    continue;
                }

                float d1 = signed_distance_to_point(plane, tri.vertex[VERTEX_A]);
                float d2 = signed_distance_to_point(plane, tri.vertex[VERTEX_B]);
                float d3 = signed_distance_to_point(plane, tri.vertex[VERTEX_C]);

                if (d1 > FLT_MIN && d2 > FLT_MIN && d3 > FLT_MIN) {
                    clipped->trisClipped[clipped->trisClippedCount] = tri;
                    clipped->trisClippedCount++;
                    
                    continue;
                }
                
                // TODO: tris will potentially* have a value for brightness for each vertex
                // "must" compute intermediate value from P1 to P2 in case of clipping tri
                Vec3 pointA = tri.vertex[VERTEX_A];
                Vec3 pointB = tri.vertex[VERTEX_B];
                Vec3 pointC = tri.vertex[VERTEX_C];

                if (d1 > FLT_MIN && d2 < 0.0f && d3 < 0.0f) {
                    one_vertex_in_front(plane, clipped, clipped->trisClippedCount, pointA, pointB, pointC, tri, VERTEX_B, VERTEX_C);
                    continue;
                }
                if (d1 < 0.0f && d2 > FLT_MIN && d3 < 0.0f) {
                    one_vertex_in_front(plane, clipped, clipped->trisClippedCount, pointB, pointA, pointC, tri, VERTEX_A, VERTEX_C);
                    continue;
                }
                if (d1 < 0.0f && d2 < 0.0f && d3 > FLT_MIN) {
                    one_vertex_in_front(plane, clipped, clipped->trisClippedCount, pointC, pointA, pointB, tri, VERTEX_A, VERTEX_B);
                    continue;
                }

                // [Triangle(A, B, A'), Triangle(A', B, B')]
                if (d1 > FLT_MIN && d2 > FLT_MIN && d3 < 0.0f) {
                    two_vertices_in_front(
                        plane, clipped, pointA, pointB, pointC, tri,
                        VERTEX_C, VERTEX_A, clipped->trisClippedCount, endBufferIndex + trisLength
                    );
                    endBufferIndex++;
                    continue;
                }

                // [Triangle(A, A', C), Triangle(A', C', C)]
                if (d1 > FLT_MIN && d2 < 0.0f && d3 > FLT_MIN) {
                    two_vertices_in_front(
                        plane, clipped, pointA, pointC, pointB, tri,
                        VERTEX_B, VERTEX_A, clipped->trisClippedCount, endBufferIndex + trisLength
                    );   
                    endBufferIndex++;
                    continue;
                }

                // [Triangle(C', B, C), Triangle(B', B, C')]
                if (d1 < 0.0f && d2 > FLT_MIN && d3 > FLT_MIN) {
                    two_vertices_in_front(
                        plane, clipped, pointC, pointB, pointA, tri,
                        VERTEX_A, VERTEX_C, clipped->trisClippedCount, endBufferIndex + trisLength
                    );
                    endBufferIndex++;
                    continue;
                }               
            }
            
            
            if (clipped->trisClippedCount == 0) {
                clipped = NULL;
                break;
            }
            
            for(size_t i = 0; i < endBufferIndex; i++) {
                clipped->trisClipped[i+clipped->trisClippedCount] = clipped->trisClipped[i+trisLength];
            }
            clipped->trisClippedCount += endBufferIndex;
        }

        if (clipped != NULL) {
            render_model(c, *clipped);
        } 
    }
}