#ifndef _MODEL_3D_H
#define _MODEL_3D_H

#include <stddef.h>

#include "transforms.h"
#include "raylib.h"

typedef struct Triangle {
    size_t v1, v2, v3;
    Color color;
} Triangle;

typedef struct ModelData {   
    size_t vertsCount;
    size_t trisCount;
    Vec3 *verts;
    Triangle *tris;
} ModelData ;

typedef struct Instance {
    ModelData *model;
    Vec3 rotation;
    Vec3 scale;
    Vec3 translation;
} Instance;

#endif