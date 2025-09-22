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

void draw_line(Cam c, Point pos0, Point pos1 , Color color){
    int dx = pos1.x - pos0.x;
    int dy = pos1.y - pos0.y;
    // printf("--------dx: %d dy: %d---------\n", dx, dy);

    if (abs(dx) > abs(dy)) {
        if (dx < 0) {
            swap_point_values(&pos0, &pos1);
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
        swap_point_values(&pos0, &pos1);
    }

    float *xs = interpolate(pos0.y, pos0.x, pos1.y, pos1.x);

    for(int y = pos0.y; y <= pos1.y; y++) {
        // printf("dx < dy - xs[%d]:%f y:%d\n", y - pos0.y, xs[y - pos0.y], y);
        
        put_pixel(c, color, xs[y - pos0.y], y);
    }

    free(xs);
}

void draw_wireframe_triangle(Cam c, Point p0, Point p1, Point p2, Color color) {
    // printf("p1: %dx%d -> p2: %dx%d -> p3: %dx%d Color: R%dG%dB%dA%d\n", 
    //     p0.x, p0.y, p1.x, p1.y, p2.x, p2.y, color.r, color.g, color.b, color.a);
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
    
    float *x12 = interpolate(p1.y, p1.x, p2.y, p2.x); // values from p1.x to p2.x included 
    float *h12 = interpolate(p1.y, p1.brightness, p2.y, p2.brightness); // values from p1.brightness to p2.brightness included
    
    float *x02 = interpolate(p0.y, p0.x, p2.y, p2.x); // values from p0.x to p2.x included
    float *h02 = interpolate(p0.y, p0.brightness, p2.y, p2.brightness); // values from p0.brightness to p2.brightness included

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

void render_triangle(Cam c, Triangle tri, const Point *projected) {
    draw_wireframe_triangle(
        c, 
        projected[tri.v1],
        projected[tri.v2],
        projected[tri.v3], 
        tri.color);
}

float signed_distance_to_point(ViewPlane plane, Vec3 vertex) {
    float d = vec3_by_vec3_multiply(plane.normal, vertex);
    d += plane.d;
    return d;
}

// TODO: Get projected point brightness from triangle
void render_model(Cam c, Instance instance) {
    Point *points = malloc(sizeof(Point)*instance.model->vertsCount);

    if (instance.model->trisClippedCount > 0) {
        for (size_t i = 0; i < instance.model->trisClippedCount; i++){
            Triangle t = instance.model->trisClipped[i];
            // printf("%lld:tri render: v1: %lld v2: %lld v3: %lld\n", i, t.v1, t.v2, t.v3);
            points[t.v1] = project_vertex(c, instance.model->vertsWorld[t.v1]);
            points[t.v2] = project_vertex(c, instance.model->vertsWorld[t.v2]);
            points[t.v3] = project_vertex(c, instance.model->vertsWorld[t.v3]);
        }
        for (size_t i = 0; i < instance.model->trisClippedCount; i++){
            render_triangle(c, instance.model->trisClipped[i], points);
        }
    } else {
        // puts("full render");
        for (size_t i = 0; i < instance.model->vertsCount; i++){
            points[i] = project_vertex(c, instance.model->vertsWorld[i]);
        }

        for (size_t i = 0; i < instance.model->trisCount; i++) {
            render_triangle(c, instance.model->tris[i], points);
        }
    }

    free(points);
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

void render_scene(Cam c, Scene scene) {
    float m_transform[M4X4];

    for(size_t i = 0; i < scene.objectCount; i++){        
        Instance *clipped = &scene.instances[i];

        matrix_multiplication(c.matrixTransform, clipped->matrixTransform, m_transform);
        clipped->boundingSphere.centerWorld = mult_matrix_by_vec3(m_transform, clipped->boundingSphere.center);

        if (!is_inside_frustum(c.frustum, *clipped)) {
            continue;
        }

        for (size_t n = 0; n < clipped->model->vertsCount; n++) {
            clipped->model->vertsWorld[n] = mult_matrix_by_vec3(m_transform, clipped->model->verts[n]);
        }

        clipped->model->trisClippedCount = 0;
        for (size_t j = 0; j < FRUSTUM_PLANES; j++) {
            ViewPlane plane = c.frustum[j];

            float d = signed_distance_to_point(plane, clipped->boundingSphere.centerWorld);
            float r = clipped->boundingSphere.radius;
           

            if ( d > r ) {
                continue;
            }

            // TODO: generate intermediate tris

            size_t trisLength = clipped->model->trisCount;
            Triangle *tris = clipped->model->tris;

            if (clipped->model->trisClippedCount > 0){
                trisLength = clipped->model->trisClippedCount;
                tris = clipped->model->trisClipped;
            }

            clipped->model->trisClippedCount = 0;
            // clipped->model->vertsClippedCount = clipped->model->vertsCount;
            for (size_t n = 0; n < trisLength; n++) {
                Triangle t = tris[n];
            
                float d1 = signed_distance_to_point(plane, clipped->model->vertsWorld[t.v1]);
                float d2 = signed_distance_to_point(plane, clipped->model->vertsWorld[t.v2]);
                float d3 = signed_distance_to_point(plane, clipped->model->vertsWorld[t.v3]);

                if (d1 > FLT_MIN && d2 > FLT_MIN && d3 > FLT_MIN) {
                    // printf("d1: %g d2: %g d3: %g\n", d1, d2, d3),
                    clipped->model->trisClipped[clipped->model->trisClippedCount] = t;
                    clipped->model->trisClippedCount++;
                    continue;
                }
                
                // TODO: tris will potentially* have a value for brightness for each vertex
                // "must" compute intermediate value from P1 to P2 in case of clipping tri
                Vec3 pointA = clipped->model->vertsWorld[t.v1];
                Vec3 pointB = clipped->model->vertsWorld[t.v2];
                Vec3 pointC = clipped->model->vertsWorld[t.v3];

                // One vextex in front of view plane
                if (d1 > FLT_MIN && d2 < 0.0f && d3 < 0.0f) {
                    // printf("Just A %lld tri n:%lld\n",t.v1, n);
                    Vec3 pointBprime = intersection_on_plane(plane, pointA, pointB);
                    Vec3 pointCprime = intersection_on_plane(plane, pointA, pointC);

                    clipped->model->vertsWorld[t.v2] = pointBprime;
                    clipped->model->vertsWorld[t.v3] = pointCprime;

                    clipped->model->trisClipped[clipped->model->trisClippedCount] = t;
                    clipped->model->trisClippedCount++;
                    continue;
                }
                if (d1 < 0.0f && d2 > FLT_MIN && d3 < 0.0f) {
                    // printf("Just B %lld tri n:%lld\n",t.v2, n);
                    Vec3 pointAprime = intersection_on_plane(plane, pointB, pointA);
                    Vec3 pointCprime = intersection_on_plane(plane, pointB, pointC);

                    clipped->model->vertsWorld[t.v1] = pointAprime;
                    clipped->model->vertsWorld[t.v3] = pointCprime;

                    clipped->model->trisClipped[clipped->model->trisClippedCount] = t;
                    clipped->model->trisClippedCount++;
                    continue;
                }
                if (d1 < 0.0f && d2 < 0.0f && d3 > FLT_MIN) {
                    // printf("Just C %lld tri n:%lld\n",t.v3, n);
                    Vec3 pointAprime = intersection_on_plane(plane, pointC, pointA);
                    Vec3 pointBprime = intersection_on_plane(plane, pointC, pointB);

                    clipped->model->vertsWorld[t.v1] = pointAprime;
                    clipped->model->vertsWorld[t.v2] = pointBprime;

                    clipped->model->trisClipped[clipped->model->trisClippedCount] = t;
                    clipped->model->trisClippedCount++;
                    continue;
                }

                // // Two vertices in front of view plane 
                // if (d1 < 0.0f && d2 > FLT_MIN && d3 > FLT_MIN) {
                //     // printf("Just A %lld tri n:%lld\n",t.v1, n);
                //     continue;
                // }
                // if (d1 > FLT_MIN && d2 < 0.0f && d3 > FLT_MIN) {
                //     printf("Just B %lld tri n:%lld\n",t.v2, n);
                // }
                if (d1 > FLT_MIN && d2 > FLT_MIN && d3 < 0.0f) {
                    // [Triangle(A, B, A'), Triangle(A', B, B')]

                    Triangle t1 = t;
                    Triangle t2 = t;

                    t1.v1 = t.v1;
                    t1.v2 = t.v2;
                    t1.v3 = t.v3;

                    t2.v1 = t.v3;
                    t2.v2 = t.v2;
                    t2.v3 = 0; // new index;

                    Vec3 pointAprime = intersection_on_plane(plane, pointA, pointC);
                    Vec3 pointBprime = intersection_on_plane(plane, pointB, pointC);

                    size_t pIndex = clipped->model->vertsClippedCount;
                    clipped->model->vertsClippedCount++;
                    for (size_t vI = 0; vI < clipped->model->vertsClippedCount; vI++) {
                        bool isIndexAvaliable = true;
                        for (size_t tI = 0; tI < trisLength; tI++) {
                            if(vI == tris[tI].v1 || vI == tris[tI].v2 || vI == tris[tI].v3) {
                                isIndexAvaliable = false;
                            }
                        }
                        if (isIndexAvaliable) {
                            pIndex = vI;
                            if (pIndex < (clipped->model->vertsClippedCount - 1)) {
                                clipped->model->vertsClippedCount--;
                            }
                            break;
                        }
                    }
                    
                    t2.v3 = pIndex;
                    // printf("%lld\n", clipped->model->vertsClippedCount);

                    t2.v1 = t1.v3;

                    clipped->model->vertsWorld[t1.v3] = pointAprime;
                    clipped->model->vertsWorld[t2.v3] = pointBprime;
                    
                    clipped->model->trisClipped[clipped->model->trisClippedCount] = t1;
                    clipped->model->trisClippedCount++;
                    
                    clipped->model->trisClipped[clipped->model->trisClippedCount] = t2;
                    clipped->model->trisClippedCount++;
                    // printf("tri index:%lld new index %lld\n", clipped->model->trisClippedCount, pIndex);
                    // print_vec3("A':", pointAprime);
                    // print_vec3("B':", pointBprime);
                    continue;
                }
            }
            if (clipped->model->trisClippedCount == 0) {
                clipped = NULL;
                break;
            }
        }

        if (clipped != NULL) {
            render_model(c, *clipped);
        } 
    }
}