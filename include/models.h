#ifndef _MODELS_H
#define _MODELS_H

#include <stddef.h>

#include "transforms.h"
#include "raylib.h"

typedef struct Triangle {
    size_t v1, v2, v3;
    Color color;
} Triangle;

typedef struct ModelData {
    size_t vertsCount;
    size_t vertsClippedCount;
    
    size_t trisCount;
    size_t trisClippedCount;
    
    Vec3 *verts;
    Vec3 *vertsWorld;
    Vec3 *vertsWorldClipped;

    Triangle *tris;
    Triangle *trisClipped;
} ModelData ;

typedef struct Sphere {
    Vec3 center;
    Vec3 centerWorld;
    float radius;
} Sphere;

typedef struct Instance {
    ModelData *model;
    Vec3 rotation;
    Vec3 scale;
    Vec3 position;
    Sphere boundingSphere;
    float *matrixTransform;
} Instance;

ModelData init_model(Vec3 *verts, size_t vertsCount, Triangle *tris, size_t trisCount);
void free_model(ModelData model);

ModelData cube_model();

void update_instance_transforms(Instance *instance);

#endif