#include <math.h>
#include "shapes.h"

#define SPHERE_SEGMENTS 16
#define SPHERE_RINGS 12
#define SPHERE_RADIUS 1.0f

#define TORUS_MAJOR_SEGMENTS 24
#define TORUS_MINOR_SEGMENTS 12
#define TORUS_MAJOR_RADIUS 1.5f
#define TORUS_MINOR_RADIUS 0.5f

// Icosahedron vertices
Vec3 verts_ico[] = {
    { 0.000f,  1.000f,  0.000f}, // 0 top
    { 0.894f,  0.447f,  0.000f}, // 1
    { 0.276f,  0.447f,  0.851f}, // 2
    {-0.724f,  0.447f,  0.526f}, // 3
    {-0.724f,  0.447f, -0.526f}, // 4
    { 0.276f,  0.447f, -0.851f}, // 5
    { 0.724f, -0.447f,  0.526f}, // 6
    {-0.276f, -0.447f,  0.851f}, // 7
    {-0.894f, -0.447f,  0.000f}, // 8
    {-0.276f, -0.447f, -0.851f}, // 9
    { 0.724f, -0.447f, -0.526f}, // 10
    { 0.000f, -1.000f,  0.000f}  // 11 bottom
};

// Icosahedron triangles
Triangle tris_ico[] = {
    // Top cap
    {.v1 = 0, .v2 = 1, .v3 = 2, .color = RED},
    {.v1 = 0, .v2 = 2, .v3 = 3, .color = RED},
    {.v1 = 0, .v2 = 3, .v3 = 4, .color = GREEN},
    {.v1 = 0, .v2 = 4, .v3 = 5, .color = YELLOW},
    {.v1 = 0, .v2 = 5, .v3 = 1, .color = YELLOW},
    
    // Middle band
    {.v1 = 1, .v2 = 2, .v3 = 6, .color = MAGENTA},
    {.v1 = 2, .v2 = 3, .v3 = 7, .color = MAGENTA},
    {.v1 = 3, .v2 = 4, .v3 = 8, .color = ORANGE},
    {.v1 = 4, .v2 = 5, .v3 = 9, .color = ORANGE},
    {.v1 = 5, .v2 = 1, .v3 = 10, .color = LIME},
    
    {.v1 = 2, .v2 = 7, .v3 = 6, .color = MAROON},
    {.v1 = 3, .v2 = 8, .v3 = 7, .color = MAROON},
    {.v1 = 4, .v2 = 9, .v3 = 8, .color = GREEN},
    {.v1 = 5, .v2 = 10, .v3 = 9, .color = GREEN},
    {.v1 = 1, .v2 = 6, .v3 = 10, .color = GOLD},
    
    // Bottom cap
    {.v1 = 11, .v2 = 7, .v3 = 6, .color = GOLD},
    {.v1 = 11, .v2 = 8, .v3 = 7, .color = BLUE},
    {.v1 = 11, .v2 = 9, .v3 = 8, .color = BLUE},
    {.v1 = 11, .v2 = 10, .v3 = 9, .color = PURPLE},
    {.v1 = 11, .v2 = 6, .v3 = 10, .color = PURPLE}
};

// CUBE === begging
Vec3 verts[]  = {
    { 1.0f,  1.0f,  1.0f}, // 0 front top right
    {-1.0f,  1.0f,  1.0f}, // 1 front top left
    {-1.0f, -1.0f,  1.0f}, // 2 front bottom left
    { 1.0f, -1.0f,  1.0f}, // 3 front bottom rigth
    { 1.0f,  1.0f, -1.0f}, // 4 back top right
    {-1.0f,  1.0f, -1.0f}, // 5 back top left
    {-1.0f, -1.0f, -1.0f}, // 6 back bottom left
    { 1.0f, -1.0f, -1.0f}, // 7 back bottom right
};

Triangle tris[] = {
    {.v1 = 0, .v2 = 1, .v3 = 2, .color = RED},
    {.v1 = 0, .v2 = 2, .v3 = 3, .color = RED},
    {.v1 = 4, .v2 = 0, .v3 = 3, .color = GREEN},
    {.v1 = 4, .v2 = 3, .v3 = 7, .color = GREEN},
    {.v1 = 5, .v2 = 4, .v3 = 7, .color = BLUE},
    {.v1 = 5, .v2 = 7, .v3 = 6, .color = BLUE},
    {.v1 = 1, .v2 = 5, .v3 = 6, .color = YELLOW},
    {.v1 = 1, .v2 = 6, .v3 = 2, .color = YELLOW},
    {.v1 = 4, .v2 = 5, .v3 = 1, .color = PURPLE},
    {.v1 = 4, .v2 = 1, .v3 = 0, .color = PURPLE},
    {.v1 = 2, .v2 = 6, .v3 = 7, .color = (Color) {.a = 255, .r = 0, .g = 255, .b = 255} },
    {.v1 = 2, .v2 = 7, .v3 = 3, .color = (Color) {.a = 255, .r = 0, .g = 255, .b = 255} },
};
// CUBE === end

ModelData cube_shape() {
    return init_model(verts, 8, tris, 12);
}

ModelData icosahedron_shape() {
    return init_model(verts_ico, 12, tris_ico, 20);
}

ModelData torus_shape() {
    Vec3 torus_verts[TORUS_MAJOR_SEGMENTS * TORUS_MINOR_SEGMENTS];
    Triangle torus_tris[TORUS_MAJOR_SEGMENTS * TORUS_MINOR_SEGMENTS * 2];

    float majorRadius = 1.0f;
    float minorRadius = 0.3f;
    for (int i = 0; i < TORUS_MAJOR_SEGMENTS; i++) {
        float majorAngle = 2.0f * T_PI * i / TORUS_MAJOR_SEGMENTS;
        float cosMajor = cosf(majorAngle);
        float sinMajor = sinf(majorAngle);
        
        for (int j = 0; j < TORUS_MINOR_SEGMENTS; j++) {
            float minorAngle = 2.0f * T_PI * j / TORUS_MINOR_SEGMENTS;
            float cosMinor = cosf(minorAngle);
            float sinMinor = sinf(minorAngle);
            
            int index = i * TORUS_MINOR_SEGMENTS + j;
            torus_verts[index].x = (majorRadius + minorRadius * cosMinor) * cosMajor;
            torus_verts[index].y = (majorRadius + minorRadius * cosMinor) * sinMajor;
            torus_verts[index].z = minorRadius * sinMinor;
        }
    }

    // Generate torus faces
    int triIndex = 0;
    for (int i = 0; i < TORUS_MAJOR_SEGMENTS; i++) {
        for (int j = 0; j < TORUS_MINOR_SEGMENTS; j++) {
            int nextI = (i + 1) % TORUS_MAJOR_SEGMENTS;
            int nextJ = (j + 1) % TORUS_MINOR_SEGMENTS;
            
            int v00 = i * TORUS_MINOR_SEGMENTS + j;
            int v01 = i * TORUS_MINOR_SEGMENTS + nextJ;
            int v10 = nextI * TORUS_MINOR_SEGMENTS + j;
            int v11 = nextI * TORUS_MINOR_SEGMENTS + nextJ;
            
            torus_tris[triIndex++] = (Triangle){.v1 = v00, .v2 = v01, .v3 = v10, .color = DARKBLUE};
            torus_tris[triIndex++] = (Triangle){.v1 = v10, .v2 = v01, .v3 = v11, .color = DARKGRAY};
        }
    }
    return init_model(
        torus_verts, 
        TORUS_MAJOR_SEGMENTS * TORUS_MINOR_SEGMENTS, 
        torus_tris, 
        TORUS_MAJOR_SEGMENTS * TORUS_MINOR_SEGMENTS * 2);
}

ModelData uv_sphere_shape() {

    Vec3 sphere_verts[SPHERE_SEGMENTS * SPHERE_RINGS];
    Triangle sphere_tris[SPHERE_SEGMENTS * (SPHERE_RINGS - 1) * 2];

    int vert_index = 0;
    for (int ring = 0; ring < SPHERE_RINGS; ring++) {
        float phi = T_PI * ring / (SPHERE_RINGS - 1);
        float sin_phi = sinf(phi);
        float cos_phi = cosf(phi);
        
        for (int seg = 0; seg < SPHERE_SEGMENTS; seg++) {
            float theta = 2.0f * T_PI * seg / SPHERE_SEGMENTS;
            float sin_theta = sinf(theta);
            float cos_theta = cosf(theta);
            
            sphere_verts[vert_index].x = SPHERE_RADIUS * sin_phi * cos_theta;
            sphere_verts[vert_index].y = SPHERE_RADIUS * cos_phi;
            sphere_verts[vert_index].z = SPHERE_RADIUS * sin_phi * sin_theta;
            vert_index++;
        }
    }

    int tri_index = 0;
    for (int ring = 0; ring < SPHERE_RINGS - 1; ring++) {
        for (int seg = 0; seg < SPHERE_SEGMENTS; seg++) {
            int next_seg = (seg + 1) % SPHERE_SEGMENTS;
            
            int v0 = ring * SPHERE_SEGMENTS + seg;
            int v1 = ring * SPHERE_SEGMENTS + next_seg;
            int v2 = (ring + 1) * SPHERE_SEGMENTS + seg;
            int v3 = (ring + 1) * SPHERE_SEGMENTS + next_seg;
            
            sphere_tris[tri_index++] = (Triangle){.v1 = v0, .v2 = v2, .v3 = v1, .color = DARKGREEN};
            sphere_tris[tri_index++] = (Triangle){.v1 = v1, .v2 = v2, .v3 = v3, .color = DARKGREEN};
        }
    }
    return init_model(
        sphere_verts, 
        SPHERE_SEGMENTS * SPHERE_RINGS,
        sphere_tris, SPHERE_SEGMENTS * (SPHERE_RINGS - 1) * 2
    );
}

ModelData square_shape() {
    Vec3 square_verts[] = {
        { 1.0f,  1.0f,  0.0f}, // 0 front top right
        {-1.0f,  1.0f,  0.0f}, // 1 front top left
        {-1.0f, -1.0f,  0.0f}, // 2 front bottom left
        { 1.0f, -1.0f,  0.0f}, // 3 front bottom rigth
    };
    Triangle square_tris[] = {
        {.v1 = 0, .v2 = 1, .v3 = 2, .color = RED},
        {.v1 = 0, .v2 = 2, .v3 = 3, .color = RED},
    };

    return init_model(square_verts, 4, square_tris, 2);
}