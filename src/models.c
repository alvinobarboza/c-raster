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
}