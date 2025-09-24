#ifndef _MODELS_H
#define _MODELS_H

#include <stddef.h>

#include "transforms.h"
#include "raylib.h"

#define VERTEX_A 0
#define VERTEX_B 1
#define VERTEX_C 2

typedef struct Triangle {
    size_t v1, v2, v3;
    Color color;
} Triangle;

typedef struct ModelData {
    Vec3 *verts;
    Triangle *tris;
    size_t trisCount;
    size_t vertsCount;    
} ModelData ;

typedef struct FullTriangle {
    Vec3 vertex[3];
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

ModelData init_model(Vec3 *verts, size_t vertsCount, Triangle *tris, size_t trisCount);
void free_model(ModelData *model);

Instance init_instance(ModelData *model, Transforms transform);
void free_instance(Instance *instance);

void update_instance_transforms(Instance *instance);

#endif