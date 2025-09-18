#include <math.h>

#include "models.h"
#include "transforms.h"



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
        float x = instance->model->verts[i].x - instance->boundingSphere.center.x;
        float y = instance->model->verts[i].y - instance->boundingSphere.center.y;
        float z = instance->model->verts[i].z - instance->boundingSphere.center.z;
        float r = sqrtf((x*x)+(y*y)+(z*z));

        if (instance->boundingSphere.radius < r) {
            instance->boundingSphere.radius = r;
        }
    }
}