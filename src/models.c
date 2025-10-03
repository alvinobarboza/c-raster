#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "models.h"
#include "transforms.h"
#include "shapes.h"

ModelData load_model_from_path(const char *pathModel, const char *pathTexture) {
    ModelData model = {0};

    FILE *fp = fopen(pathModel, "r");
    if (fp == NULL){
        return cube_shape();
    }

    char buffer[1024];
    size_t trisCount = 0;
    size_t vertsCount = 0;

    while(fgets(buffer, 1024, fp) != NULL) {
        if (buffer[0] == 'v' && buffer[1] == ' ') {
            vertsCount++;
        }
        if (buffer[0] == 'f' && buffer[1] == ' ') {
            trisCount++;

            int triCheck = 0;
            for (int i = 2; i < 1024; i++) {
                if ( buffer[i] == '\0' || buffer[i] == '\n') break;

                if ( buffer[i] == ' ' ) triCheck++;

                if ( triCheck > 2 && buffer[i] == ' ' ) trisCount++;
            }
            printf("->%ld\n", trisCount);
        }
    }

    printf("tris: %ld verts: %ld\n", trisCount, vertsCount);

    rewind(fp);

    while(fgets(buffer, 1024, fp) != NULL) {
        if (buffer[0] == 'f' && buffer[1] == ' ') {
            // printf(buffer);
        }
    }

    fclose(fp);

    return model;
}

ModelData init_model(Vec3 *vert, size_t vertsCount, Triangle *tri, size_t trisCount) {
    ModelData model;

    model.trisCount = trisCount;
    model.vertsCount = vertsCount;

    model.verts = malloc(sizeof(Vec3) * vertsCount);
    model.tris = malloc(sizeof(Triangle) * trisCount);

    for ( size_t i = 0; i < vertsCount; i++) {
        model.verts[i] = vert[i];
    }

    for ( size_t i = 0; i < trisCount; i++ ) {
        model.tris[i] = tri[i];
    }

    return model;
}

void free_model(ModelData *model) {
    if (model->verts == NULL){
        return;
    }

    free(model->verts);
    free(model->tris);

    model->verts = NULL;
    model->tris = NULL;
}

Instance init_instance(ModelData *model, Transforms transform){
    transform.matrixTransform = init_matrix();

    Instance i = (Instance) {
        .model = model,
        .transforms = transform,
        .trisWorld = malloc(sizeof(FullTriangle) * model->trisCount),
        .trisClipped = malloc(sizeof(FullTriangle) * model->trisCount * 20),
        .trisClippedCount = 0,
    };

    update_instance_transforms(&i);

    return i;
}

void free_instance(Instance *instance) {
    if (instance->trisWorld == NULL) {
        return;
    }

    free(instance->trisWorld);
    free(instance->trisClipped);
    free(instance->transforms.matrixTransform);

    instance->trisWorld = NULL;
    instance->trisClipped = NULL;
    instance->transforms.matrixTransform = NULL;
}

void update_instance_transforms(Instance *instance) {
    if (instance->transforms.matrixTransform == NULL) {
        return;
    }

    float scale[M4X4];
    float rotation[M4X4];
    float translation[M4X4];
    float result[M4X4];

    make_scale_matrix(scale, instance->transforms.scale);
    make_rotation_matrix(rotation, instance->transforms.rotation);
    make_translation_matrix(translation, instance->transforms.position);

    matrix_multiplication(rotation, scale, result);
    matrix_multiplication(translation, result, instance->transforms.matrixTransform);

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