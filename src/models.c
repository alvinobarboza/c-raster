#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "models.h"
#include "transforms.h"
#include "shapes.h"

Vec3 load_vec3(const char *buffer, const int index) {

    Vec3 tempVert = {0};
    char floatBuf[BUFFER_SIZE - 1];
    unsigned char floatLength = 0;
    unsigned char vertexPos = 0;

    for(int i = index; i < BUFFER_SIZE; i++) {
        if (buffer[i] == ' ') {

            floatBuf[floatLength] = '\0';

            float temp = strtof(floatBuf, NULL);
            if (vertexPos == 0) {
                tempVert.x = temp;
            } else {
                tempVert.y = temp;
            }
            vertexPos++;
            floatLength = 0;

            continue;
        }
        if ( buffer[i] == '\0') {
            floatBuf[floatLength+1] = '\0';

            float temp = strtof(floatBuf, NULL);
            tempVert.z = temp;
            floatLength = 0;
            break;
        }
        floatBuf[floatLength] = buffer[i];
        floatLength++;
    }

    return tempVert;
}

ModelData load_model_from_path(const char *pathModel, const char *pathTexture, bool reorder) {
    printf("Loading %s\n", pathModel);

    ModelData model = {0};

    FILE *fp = fopen(pathModel, "r");
    if (fp == NULL){
        return cube_shape();
    }

    char buffer[BUFFER_SIZE];
    size_t trisCount = 0;
    size_t vertsCount = 0;
    size_t normalCount = 0;

    while(fgets(buffer, BUFFER_SIZE, fp) != NULL) {
        if (buffer[0] == 'v' && buffer[1] == ' ') {
            vertsCount++;
        }

        if (buffer[0] == 'v' && buffer[1] == 'n' && buffer[2] == ' ') {
            normalCount++;
        }

        if (buffer[0] == 'f' && buffer[1] == ' ') {
            trisCount++;

            int triCheck = 0;
            for (int i = 2; i < BUFFER_SIZE; i++) {
                if ( buffer[i] == '\0' || buffer[i] == '\n') break;

                if ( buffer[i] == ' ' ) triCheck++;

                if ( triCheck > 2 && buffer[i] == ' ' ) trisCount++;
            }
        }
    }

    printf("tris: %ld verts: %ld normals: %ld\n", trisCount, vertsCount, normalCount);

    Triangle *tris = malloc(sizeof(Triangle) * trisCount);
    Vec3 *verts = malloc(sizeof(Vec3) * vertsCount);
    Vec3 *normals = malloc(sizeof(Vec3) * normalCount);

    rewind(fp);

    size_t vertsIndex = 0;
    size_t normalIndex = 0;

    Triangle tempTri = {0};
    tempTri.color = GRAY;
    size_t trisIndex = 0;

    char indexBuf[BUFFER_SIZE - 1];
    unsigned char indexLength = 0;

    while( fgets(buffer, BUFFER_SIZE, fp) != NULL ) {
        if (buffer[0] == 'v' && buffer[1] == ' ') {
            if (vertsIndex < vertsCount) {
                verts[vertsIndex] = load_vec3(buffer, 2);
                vertsIndex++;
            }
        }

        if (buffer[0] == 'v' && buffer[1] == 'n' && buffer[2] == ' ') {
            if (normalIndex < normalCount) {
                normals[normalIndex] = load_vec3(buffer, 3);
                normalIndex++;
            }
        }

        if (buffer[0] == 'f' && buffer[1] == ' ') {
            bool isVertexI = true;
            unsigned char indexPos = 0;
            for(int i = 2; i < BUFFER_SIZE; i++) {
                if (buffer[i] == ' ') {
                    isVertexI = true;
                    continue;
                }

                if ( buffer[i] == '\0') {
                    break;
                }

                if ( isVertexI && buffer[i] == '/' ) {
                    isVertexI = false;
                    indexBuf[indexLength] = '\0';
                    
                    switch(indexPos) {
                    case 0:
                        tempTri.v1 = atol(indexBuf);
                        tempTri.v1 += -1;
                        break;
                    case 1:
                        tempTri.v2 = atol(indexBuf);
                        tempTri.v2 += -1;
                        break;
                    case 2:
                        tempTri.v3 = atol(indexBuf);
                        tempTri.v3 += -1;
                        if (trisIndex < trisCount) {
                            tris[trisIndex] = tempTri;
                            trisIndex++;
                        }
                        break;
                    default:
                        tempTri.v2 = tempTri.v3;
                        tempTri.v3 = atol(indexBuf);
                        tempTri.v3 += -1;
                        if (trisIndex < trisCount) {
                            tris[trisIndex] = tempTri;
                            trisIndex++;
                        }
                        break;
                    }

                    indexPos++;
                    indexLength = 0;
                }

                if (isVertexI) {
                    indexBuf[indexLength] = buffer[i];
                    indexLength++;                    
                }
            }

            printf("%s", &buffer[0]);
        }
    }

    if (reorder) {
        for(size_t i = 0; i < trisCount; i++) {
            size_t temp = tris[i].v1;
            tris[i].v1 = tris[i].v3;
            tris[i].v3 = temp;
        }
    }

    model = init_model(verts, vertsCount, tris, trisCount);

    fclose(fp);
    free(tris);
    free(verts);
    free(normals);

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