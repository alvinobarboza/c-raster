#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <float.h>
#include <stdint.h>
#include "render.h"
#include "maths.h"

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

    float a = dd / (float)(abs(di));
    float d = d0;

    for (int i = i0; i <= i1; i++) {
        values[i-i0] = d;
        d = d + a;
    }

    return values;
}

void draw_line(Cam c, Point pointA, Point pointB , Color color){
    int dx = pointB.x - pointA.x;
    int dy = pointB.y - pointA.y;

    if (abs(dx) > abs(dy)) {
        if (dx < 0) {
            swap_point_values(&pointA, &pointB);
        }

        float abY = (float)(pointB.y - pointA.y) / (float)(pointB.x - pointA.x);
        float abZ = (float)(pointB.zDepth - pointA.zDepth) / (float)(pointB.x - pointA.x);

        float ys = pointA.y;
        float zs = pointA.zDepth;

        for(int x = pointA.x; x <= pointB.x; x++) {
            put_pixel(c, color, x, ys, 1, zs);
            ys += abY;
            zs += abZ - 0.2;
        }
        return;
    }

    if (dy < 0) {
        swap_point_values(&pointA, &pointB);
    }

    float abX = (float)(pointB.x - pointA.x) / (float)(pointB.y - pointA.y);
    float abZ = (float)(pointB.zDepth - pointA.zDepth) / (float)(pointB.y - pointA.y);

    float xs = pointA.x;
    float zs = pointA.zDepth;

    for(int y = pointA.y; y <= pointB.y; y++) {
        put_pixel(c, color, xs, y, 1, zs);
        xs += abX;
        zs += abZ - 0.2;
    }
}

void draw_wireframe_triangle(Cam c, Point p0, Point p1, Point p2, Color color) {
    draw_line(c, p0, p1, color);
    draw_line(c, p1, p2, color);
    draw_line(c, p2, p0, color);
}

void draw_top_bottom(Cam c, Point pointA, Point pointB, Point pointC, Color color, TextureData *texture) {
    if (pointB.x > pointC.x) swap_point_values(&pointB, &pointC);

    float lengthAB = abs(pointB.y - pointA.y);
    float lengthAC = abs(pointC.y - pointA.y);

    float abX = (pointB.x - pointA.x) / lengthAB;
    float acX = (pointC.x - pointA.x) / lengthAC;

    float abZ = (pointB.zDepth - pointA.zDepth) / lengthAB;
    float acZ = (pointC.zDepth - pointA.zDepth) / lengthAC;

    pointA.uvCoord = vec3_multiply(pointA.uvCoord, pointA.zDepth);
    pointB.uvCoord = vec3_multiply(pointB.uvCoord, pointB.zDepth);
    pointC.uvCoord = vec3_multiply(pointC.uvCoord, pointC.zDepth);

    Vec3 abUV = vec3_divide(vec3_sub(pointB.uvCoord, pointA.uvCoord), lengthAB);
    Vec3 acUV = vec3_divide(vec3_sub(pointC.uvCoord, pointA.uvCoord), lengthAC);

    float xLeft = pointA.x;
    float xRight = pointA.x;

    float zLeft = pointA.zDepth;
    float zRight = pointA.zDepth;

    Vec3 uvLeft = pointA.uvCoord;
    Vec3 uvRight = pointA.uvCoord;
    Vec3 uvInner = (Vec3) {0};

    for(int scanlineY = pointA.y; scanlineY <= pointC.y; scanlineY++) {

        float xZ = zLeft;
        Vec3 uvStep = {0};
        if (xRight != xLeft) {
            float xLength = xRight - xLeft;
            xZ = (zRight - zLeft) / xLength;

            uvStep = vec3_divide(vec3_sub(uvRight, uvLeft), xLength);
        }
        float depth = zLeft;
        uvInner = uvLeft;

        if (texture != NULL && texture->colors != NULL) {
            color = texel_from_texture(texture, uvInner.x/depth, uvInner.y/depth );
        }

        put_pixel(c, color, xLeft, scanlineY, 1, depth);
        for(int x = xLeft; x <= xRight; x++){
            put_pixel(c, color, x, scanlineY, 1, depth);
            if (xRight != xLeft) {
                depth += xZ;
                uvInner = vec3_add(uvInner, uvStep);
            }

            if (texture != NULL && texture->colors != NULL) {
                color = texel_from_texture(texture, uvInner.x/depth, uvInner.y/depth );
            }
        }
        put_pixel(c, color, xRight, scanlineY, 1, depth);

        xLeft += abX;
        xRight += acX;

        zLeft += abZ;
        zRight += acZ;

        uvLeft = vec3_add(uvLeft, abUV);
        uvRight = vec3_add(uvRight, acUV);
    }
}

void draw_bottom_top(Cam c, Point pointA, Point pointB, Point pointC, Color color, TextureData *texture) {
    if (pointB.x < pointA.x) swap_point_values(&pointB, &pointA);

    float lengthCA = abs(pointC.y - pointA.y);
    float lengthCB = abs(pointC.y - pointB.y);

    float caX = (pointC.x - pointA.x) / lengthCA;
    float cbX = (pointC.x - pointB.x) / lengthCB;

    float caZ = (pointC.zDepth - pointA.zDepth) / lengthCA;
    float cbZ = (pointC.zDepth - pointB.zDepth) / lengthCB;

    pointA.uvCoord = vec3_multiply(pointA.uvCoord, pointA.zDepth);
    pointB.uvCoord = vec3_multiply(pointB.uvCoord, pointB.zDepth);
    pointC.uvCoord = vec3_multiply(pointC.uvCoord, pointC.zDepth);

    Vec3 caUV = vec3_divide(vec3_sub(pointC.uvCoord, pointA.uvCoord), lengthCA);
    Vec3 cbUV = vec3_divide(vec3_sub(pointC.uvCoord, pointB.uvCoord), lengthCB);

    float xLeft = pointC.x;
    float xRight = pointC.x;

    float zLeft = pointC.zDepth;
    float zRight = pointC.zDepth;

    Vec3 uvLeft = pointC.uvCoord;
    Vec3 uvRight = pointC.uvCoord;
    Vec3 uvInner = (Vec3) {0};

    for(int scanlineY = pointC.y; scanlineY >= pointA.y; scanlineY--) {

        float xZ = zLeft;
        Vec3 uvStep = {0};
        if (xRight != xLeft) {
            float xLength = xRight - xLeft;
            xZ = (zRight - zLeft) / xLength;

            uvStep = vec3_divide(vec3_sub(uvRight, uvLeft), xLength);
        }
        float depth = zLeft;
        uvInner = uvLeft;

        if (texture != NULL && texture->colors != NULL) {
            color = texel_from_texture(texture, uvInner.x/depth, uvInner.y/depth);
        }

        put_pixel(c, color, xLeft, scanlineY, 1, depth);
        for(int x = xLeft; x <= xRight; x++) {

            put_pixel(c, color, x, scanlineY, 1, depth);
            if (xRight != xLeft) {
                depth += xZ;
                uvInner = vec3_add(uvInner, uvStep);                
            }

            if (texture != NULL && texture->colors != NULL) {
                color = texel_from_texture(texture, uvInner.x/depth, uvInner.y/depth);
            }
        }
        put_pixel(c, color, xRight, scanlineY, 1, depth);

        xLeft -= caX;
        xRight -= cbX;

        zLeft -= caZ;
        zRight -= cbZ;

        uvLeft = vec3_sub(uvLeft, caUV);
        uvRight = vec3_sub(uvRight, cbUV);
    }
}


void draw_filled_triangle(Cam c, Point pointA, Point pointB, Point pointC, Color color, TextureData *texture) {
    // Sort the points so that pointA.y <= pointB.y <= pointC.y
    if (pointB.y < pointA.y) swap_point_values(&pointB, &pointA);
    if (pointC.y < pointA.y) swap_point_values(&pointC, &pointA);
    if (pointC.y < pointB.y) swap_point_values(&pointC, &pointB);

    if (pointB.y == pointC.y) {
        draw_top_bottom(c, pointA, pointB, pointC, color, texture);
    } else if (pointA.y == pointB.y) {
        draw_bottom_top(c, pointA, pointB, pointC, color, texture);
    } else {

        float t = (float)(pointB.y - pointA.y) / (float)(pointC.y - pointA.y);
        float acZ = lerp_a_b(pointA.zDepth, pointC.zDepth, t);

        Vec3 aUV = vec3_multiply(pointA.uvCoord, pointA.zDepth);
        Vec3 cUV = vec3_multiply(pointC.uvCoord, pointC.zDepth);
        Vec3 acUV = vec3_lerp_a_b(aUV, cUV, t);

        Point pointAC = (Point) {
            .x = lerp_a_b(pointA.x, pointC.x, t), 
            .y = pointB.y,
            .zDepth = acZ,
            .uvCoord = vec3_divide(acUV, acZ),
            .normal = vec3_lerp_a_b(pointA.normal, pointC.normal, t)
        };

        draw_top_bottom(c, pointA, pointB, pointAC, color, texture);
        draw_bottom_top(c, pointB, pointAC, pointC, color, texture);        
    }
}

void render_triangle(Cam c, FullTriangle tri) {
    Point pointA = project_vertex(c, tri.vertex[VERTEX_A], tri.normal[VERTEX_A], tri.uv[VERTEX_A]);
    Point pointB = project_vertex(c, tri.vertex[VERTEX_B], tri.normal[VERTEX_B], tri.uv[VERTEX_B]);
    Point pointC = project_vertex(c, tri.vertex[VERTEX_C], tri.normal[VERTEX_C], tri.uv[VERTEX_C]);

    draw_filled_triangle(
        c, 
        pointA,
        pointB,
        pointC, 
        tri.color,
        tri.texture);
    
    // draw_wireframe_triangle(
    //     c, 
    //     pointA,
    //     pointB,
    //     pointC, 
    //     BLACK);
}

// Book treats each vertex as the normal vector from the camera, but the calculation should be 
// from tri's nomal vector to camera's direction, that's why it does the cross product that way
// If I want to maintain compatibility with actual normal vector loaded from OBJ I can flip the normals
// or flip vertex orientation to match the model "angle from normal vector to camera's normal vector position"
// Figure 12-9 has the correct model relation, but the caculation is done considering the vector 
// is from C to the normal angle, in another words, backwards
// I'm doing this way for now, maybe flipping will be bad for shading later
bool back_face_culling(FullTriangle tri) {
    float angleA = vec3_by_vec3_multiply(tri.normal[VERTEX_A],  vec3_multiply(tri.vertex[VERTEX_A], -1));
    float angleB = vec3_by_vec3_multiply(tri.normal[VERTEX_B],  vec3_multiply(tri.vertex[VERTEX_B], -1));
    float angleC = vec3_by_vec3_multiply(tri.normal[VERTEX_C],  vec3_multiply(tri.vertex[VERTEX_C], -1));
    return angleA >= 0 || angleB >= 0 || angleC >= 0;
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
    for (uint8_t j = 0; j < FRUSTUM_PLANES; j++) {
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

// Formula: This is a "lerp" function "who knew..."
// Q = A + t(B−A)
// Q > Actual point on plane 
// t > Intersection ratio
// A > Vertex in front of the positive plane's normal vector
// B > Vexter behind plane's normal vector 
Vec3 point_on_plane_from_ab(Vec3 inFrontA, Vec3 behindB, float t) {
    return vec3_lerp_a_b(inFrontA, behindB, t);
}

void one_vertex_in_front( 
    ViewPlane plane, Instance *clipped, FullTriangle t, 
    uint8_t inFrontA, uint8_t newB, uint8_t newC ) {

    float ratioAB = ratio_ab_from_plane(plane, t.vertex[inFrontA], t.vertex[newB]);
    float ratioAC = ratio_ab_from_plane(plane, t.vertex[inFrontA], t.vertex[newC]);

    Vec3 pointBprime = point_on_plane_from_ab(t.vertex[inFrontA], t.vertex[newB], ratioAB);
    Vec3 normalBprime = point_on_plane_from_ab(t.normal[inFrontA], t.normal[newB], ratioAB);
    Vec3 uvBprime = point_on_plane_from_ab(t.uv[inFrontA], t.uv[newB], ratioAB);

    Vec3 pointCprime = point_on_plane_from_ab(t.vertex[inFrontA], t.vertex[newC], ratioAC);
    Vec3 normalCprime = point_on_plane_from_ab(t.normal[inFrontA], t.normal[newC], ratioAC);
    Vec3 uvCprime = point_on_plane_from_ab(t.uv[inFrontA], t.uv[newC], ratioAC);

    t.vertex[newB] = pointBprime;
    t.normal[newB] = vec3_normal(normalBprime);
    t.uv[newB] = uvBprime;

    t.vertex[newC] = pointCprime;
    t.normal[newC] = vec3_normal(normalCprime);
    t.uv[newC] = uvCprime;

    clipped->trisClipped[clipped->trisClippedCount] = t;
    clipped->trisClippedCount++;
}

void two_vertices_in_front(
    ViewPlane plane, Instance *clipped, uint8_t vA, uint8_t vB, uint8_t vC, FullTriangle t, 
    uint8_t sharedIndex, uint8_t newIndex, size_t indexAhead ) {
    
    FullTriangle t1 = t;
    FullTriangle t2 = t;

    float ratioAC = ratio_ab_from_plane(plane, t.vertex[vA], t.vertex[vC]);
    float ratioBC = ratio_ab_from_plane(plane, t.vertex[vB], t.vertex[vC]);

    Vec3 pointAprime = point_on_plane_from_ab(t.vertex[vA], t.vertex[vC], ratioAC);
    Vec3 normalAprime = point_on_plane_from_ab(t.normal[vA], t.normal[vC], ratioAC);
    Vec3 uvAprime = point_on_plane_from_ab(t.uv[vA], t.uv[vC], ratioAC);

    Vec3 pointBprime = point_on_plane_from_ab(t.vertex[vB], t.vertex[vC], ratioBC);
    Vec3 normalBprime = point_on_plane_from_ab(t.normal[vB], t.normal[vC], ratioBC);
    Vec3 uvBprime = point_on_plane_from_ab(t.uv[vB], t.uv[vC], ratioBC);
    
    t1.vertex[sharedIndex] = pointAprime;
    t1.normal[sharedIndex] = vec3_normal(normalAprime);
    t1.uv[sharedIndex] = uvAprime;

    t2.vertex[newIndex] = pointAprime;
    t2.normal[newIndex] = vec3_normal(normalAprime);
    t2.uv[newIndex] = uvAprime;

    t2.vertex[sharedIndex] = pointBprime;
    t2.normal[sharedIndex] = vec3_normal(normalBprime);
    t2.uv[sharedIndex] = uvBprime;

    clipped->trisClipped[clipped->trisClippedCount] = t1;
    clipped->trisClipped[indexAhead] = t2;

    clipped->trisClippedCount++;
}

void render_scene(Cam c, Scene scene) {
    float m_transform[M4X4];
    float normalRotation[M4X4];
    float normalRotationTransposed[M4X4];

    make_rotation_matrix(normalRotation, c.rotation);
    matrix_transpose(normalRotation, normalRotationTransposed);

    for(size_t i = 0; i < scene.objectCount; i++){
        // if ( i > 1) {
        //     continue;
        // }
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

            clipped->trisWorld[n].texture = NULL;
            if(clipped->model->uvsCount){
                clipped->trisWorld[n].texture = clipped->model->texture;
                clipped->trisWorld[n].uv[VERTEX_A] = clipped->model->uvs[tp.t1];
                clipped->trisWorld[n].uv[VERTEX_B] = clipped->model->uvs[tp.t2];
                clipped->trisWorld[n].uv[VERTEX_C] = clipped->model->uvs[tp.t3];
            }

            // If model was loaded from file, it can have their own normals
            // Otherwise, calculate based on vertex position
            if (clipped->fromObj) {
                clipped->trisWorld[n].normal[VERTEX_A] = mult_matrix_by_vec3(normalRotationTransposed, clipped->model->normals[tp.n1]);
                clipped->trisWorld[n].normal[VERTEX_B] = mult_matrix_by_vec3(normalRotationTransposed, clipped->model->normals[tp.n2]);
                clipped->trisWorld[n].normal[VERTEX_C] = mult_matrix_by_vec3(normalRotationTransposed, clipped->model->normals[tp.n3]);
                continue;
            }

            Vec3 v1 = vec3_sub(clipped->trisWorld[n].vertex[VERTEX_B], clipped->trisWorld[n].vertex[VERTEX_A]);
            Vec3 v2 = vec3_sub(clipped->trisWorld[n].vertex[VERTEX_C], clipped->trisWorld[n].vertex[VERTEX_A]);

            // Cross product here is flipped in relation to the book
            // Backface culling method "back_face_culling" explains this divergence from the book
            Vec3 n1 = vec3_cross(v2, v1);

            clipped->trisWorld[n].normal[VERTEX_A] = n1;
            clipped->trisWorld[n].normal[VERTEX_B] = n1;
            clipped->trisWorld[n].normal[VERTEX_C] = n1;            
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

                if (d1 > FLT_MIN && d2 < 0.0f && d3 < 0.0f) {
                    one_vertex_in_front(plane, clipped, tri, VERTEX_A, VERTEX_B, VERTEX_C);
                    continue;
                }
                if (d1 < 0.0f && d2 > FLT_MIN && d3 < 0.0f) {
                    one_vertex_in_front(plane, clipped, tri, VERTEX_B, VERTEX_A, VERTEX_C);
                    continue;
                }
                if (d1 < 0.0f && d2 < 0.0f && d3 > FLT_MIN) {
                    one_vertex_in_front(plane, clipped, tri, VERTEX_C, VERTEX_A, VERTEX_B);
                    continue;
                }

                // [Triangle(A, B, A'), Triangle(A', B, B')]
                if (d1 > FLT_MIN && d2 > FLT_MIN && d3 < 0.0f) {
                    two_vertices_in_front(
                        plane, clipped, VERTEX_A, VERTEX_B, VERTEX_C, tri,
                        VERTEX_C, VERTEX_A, endBufferIndex + trisLength
                    );
                    endBufferIndex++;
                    continue;
                }

                // [Triangle(A, A', C), Triangle(A', C', C)]
                if (d1 > FLT_MIN && d2 < 0.0f && d3 > FLT_MIN) {
                    two_vertices_in_front(
                        plane, clipped, VERTEX_A, VERTEX_C, VERTEX_B, tri,
                        VERTEX_B, VERTEX_A, endBufferIndex + trisLength
                    );   
                    endBufferIndex++;
                    continue;
                }

                // [Triangle(C', B, C), Triangle(B', B, C')]
                if (d1 < 0.0f && d2 > FLT_MIN && d3 > FLT_MIN) {
                    two_vertices_in_front(
                        plane, clipped, VERTEX_C, VERTEX_B, VERTEX_A, tri,
                        VERTEX_A, VERTEX_C, endBufferIndex + trisLength
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