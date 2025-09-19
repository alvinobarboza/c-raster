#include <math.h>
#include <stdlib.h>

#include "models.h"
#include "transforms.h"

Vec3 verts[]  = {
    { 1.0f,  1.0f,  1.0f},
    {-1.0f,  1.0f,  1.0f},
    {-1.0f, -1.0f,  1.0f},
    { 1.0f, -1.0f,  1.0f},
    { 1.0f,  1.0f, -1.0f},
    {-1.0f,  1.0f, -1.0f},
    {-1.0f, -1.0f, -1.0f},
    { 1.0f, -1.0f, -1.0f},
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

ModelData init_model(Vec3 *vert, size_t vertsCount, Triangle *tri, size_t trisCount) {
    ModelData model;

    model.trisCount = trisCount;
    model.trisClippedCount = 0;

    model.vertsCount = vertsCount;
    model.vertsClippedCount = 0;

    model.tris = malloc(sizeof(Triangle) * trisCount);
    model.trisClipped = malloc(sizeof(Triangle) * (trisCount+trisCount/3));

    model.verts = malloc(sizeof(Vec3) * vertsCount);
    model.vertsWorld = malloc(sizeof(Vec3) * (vertsCount+vertsCount/3));
    model.vertsWorldClipped = malloc(sizeof(Vec3) * (vertsCount+vertsCount/3));

    for ( size_t i = 0; i < vertsCount; i++) {
        model.verts[i] = vert[i];
        model.vertsWorld[i] = vert[i];
    }
    
    for ( size_t i = 0; i < (vertsCount+vertsCount/3); i++) {
        model.vertsWorldClipped[i] = (Vec3){0};
    }

    for ( size_t i = 0; i < trisCount; i++ ) {
        model.tris[i] = tri[i];
    }
    
    for ( size_t i = 0; i < (vertsCount+vertsCount/3); i++) {
        model.trisClipped[i] = (Triangle){0};
    }    

    return model;
}

void free_model(ModelData model) {
    free(model.verts);
    free(model.vertsWorld);
    free(model.vertsWorldClipped);
    free(model.tris);
    free(model.trisClipped);
}

ModelData cube_model() {
    return init_model(verts, 8, tris, 12);
}

void update_instance_transforms(Instance *instance) {
    if (instance->matrixTransform == NULL) {
        return;
    }

    float scale[M4X4];
    float rotation[M4X4];
    float translation[M4X4];
    float result[M4X4];

    make_scale_matrix(scale, instance->scale);
    make_rotation_matrix(rotation, instance->rotation);
    make_translation_matrix(translation, instance->position);

    matrix_multiplication(rotation, scale, result);
    matrix_multiplication(translation, result, instance->matrixTransform);

    instance->boundingSphere.center = (Vec3){0};
    instance->boundingSphere.radius = 0;

    for (size_t i = 0; i < instance->model->vertsCount; i++) {
        instance->boundingSphere.center = vec3_add(
            instance->boundingSphere.center, 
            instance->model->verts[i]
        );
    }
    
    instance->boundingSphere.center.x = instance->boundingSphere.center.x / instance->model->vertsCount;
    instance->boundingSphere.center.y = instance->boundingSphere.center.y / instance->model->vertsCount;
    instance->boundingSphere.center.z = instance->boundingSphere.center.z / instance->model->vertsCount;

    for (size_t i = 0; i < instance->model->vertsCount; i++) {
        Vec3 scaled = mult_matrix_by_vec3(scale, instance->model->verts[i]);

        float x = scaled.x - instance->boundingSphere.center.x;
        float y = scaled.y - instance->boundingSphere.center.y;
        float z = scaled.z - instance->boundingSphere.center.z;
        float r = sqrtf((x*x)+(y*y)+(z*z));

        if (instance->boundingSphere.radius < r) {
            instance->boundingSphere.radius = r;
        }
    }
}