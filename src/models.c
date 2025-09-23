#include <math.h>
#include <stdlib.h>

#include "models.h"
#include "transforms.h"

ModelData init_model(Vec3 *vert, size_t vertsCount, Triangle *tri, size_t trisCount) {
    ModelData model;

    model.trisCount = trisCount;
    model.trisClippedCount = 0;

    model.vertsCount = vertsCount;
    model.vertsClippedCount = vertsCount;

    model.tris = malloc(sizeof(Triangle) * trisCount);

    // maybe impossible but if every triangle needs another triangle from clipping
    model.trisClipped = malloc(sizeof(Triangle) * (trisCount*2)); 

    model.verts = malloc(sizeof(Vec3) * vertsCount);

    // maybe impossible but if every triangle needs another triangle from clipping
    // One new vertice will be created
    model.vertsWorld = malloc(sizeof(Vec3) * (vertsCount+trisCount));

    for ( size_t i = 0; i < vertsCount; i++) {
        model.verts[i] = vert[i];
        model.vertsWorld[i] = vert[i];
    }

    for ( size_t i = 0; i < trisCount; i++ ) {
        model.tris[i] = tri[i];
    }
    
    for ( size_t i = 0; i < (trisCount*2); i++) {
        model.trisClipped[i] = (Triangle){0};
    }    

    return model;
}

void free_model(ModelData *model) {
    if (model->verts == NULL){
        return;
    }

    free(model->verts);
    free(model->vertsWorld);
    free(model->tris);
    free(model->trisClipped);
    model->verts = NULL;
    model->vertsWorld = NULL;
    model->tris = NULL;
    model->trisClipped = NULL;
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