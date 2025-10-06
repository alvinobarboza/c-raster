#ifndef _MODELS_H
#define _MODELS_H

#include <stddef.h>

#include "transforms.h"
#include "raylib.h"

#define VERTEX_A 0
#define VERTEX_B 1
#define VERTEX_C 2
#define BUFFER_SIZE 1024

typedef struct Triangle {
    size_t v1, v2, v3;
    size_t n1, n2, n3;
    size_t t1, t2, t3;
    Color color;
} Triangle;

typedef struct ModelData {
    Vec3 *verts;
    size_t vertsCount;

    Vec3 *normals;
    size_t normalsCount;

    Vec2 *uvs;
    size_t uvsCount;

    Triangle *tris;
    size_t trisCount;
} ModelData ;

typedef struct FullTriangle {
    Vec3 vertex[3];
    Vec3 normal[3];
    Vec2 uv[2];
    Color color;
} FullTriangle;

typedef struct Sphere {
    Vec3 center;
    Vec3 centerWorld;
    float radius;
} Sphere;

typedef struct Instance {
    ModelData *model;
    Transforms transforms;
    Sphere boundingSphere;
    FullTriangle *trisWorld;
    FullTriangle *trisClipped;
    size_t trisClippedCount;
} Instance;

// Not full OBJ parser, just v, vn, vt and f data - 
// That's not the point of this project
ModelData load_model_from_path(const char *pathModel, const char *pathTexture, bool reorder);

ModelData init_model(
    Vec3 *verts, size_t vertsCount, Triangle *tris, size_t trisCount, Vec3 *normals, size_t normalsCount);
void free_model(ModelData *model);

Instance init_instance(ModelData *model, Transforms transform);
void free_instance(Instance *instance);

void update_instance_transforms(Instance *instance);

#endif