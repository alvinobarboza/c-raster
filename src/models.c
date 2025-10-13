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
        if (buffer[i] == ' ' || buffer[i] == '\n' || buffer[i] == '\0') {

            floatBuf[floatLength] = '\0';

            float temp = strtof(floatBuf, NULL);
            switch(vertexPos){
            case 0:
                tempVert.x = temp;
                break;
            case 1:
                tempVert.y = temp;
                break;
            case 2:
                tempVert.z = temp;
                break;
            }

            if (buffer[i+1] == '\r' || 
                buffer[i+1] == '\n' || 
                buffer[i+1] == '\0' || 
                buffer[i] == '\n' || 
                buffer[i] == '\0') {
                break;
            }

            vertexPos++;
            floatLength = 0;
            continue;
        }

        floatBuf[floatLength] = buffer[i];
        floatLength++;
    }

    return tempVert;
}

TextureData *load_default_texture() {
    uint16_t w = 1000;
    uint16_t h = 1000;
    TextureData *texture = malloc(sizeof(TextureData));
    texture->colors = malloc(sizeof(Color) * w * h);
    texture->width = w;
    texture->height = h;

    int tile = w / 7;

    Color currentY = DARKGRAY;
    Color oldY = RAYWHITE;
    for(uint16_t y = 0; y < h; y++) {
        if (y % tile == 0 ){
            Color temp = currentY;
            currentY = oldY;
            oldY = temp;
        }
        Color current = currentY;
        Color old = oldY;
        for(uint16_t x = 0; x < w; x++) {
            if (x % tile == 0 ){
                Color temp = current;
                current = old;
                old = temp;
            }
            texture->colors[y*w+x] = current;
        }   
    }
    return texture;
}

ModelData load_model_from_path(const char *pathModel, const char *pathTexture, bool reorder, bool flipNormals, bool loadDefatulTex) {
    if (pathModel == NULL) {
        return cube_shape();
    }

    printf("Loading %s\n", pathModel);

    ModelData model = {0};
    TextureData *texture = NULL;

    FILE *fp = fopen(pathModel, "r");
    if (fp == NULL){
        return cube_shape();
    }

    char buffer[BUFFER_SIZE];
    size_t trisCount = 0;
    size_t vertsCount = 0;
    size_t normalCount = 0;
    size_t uvsCount = 0;

    while(fgets(buffer, BUFFER_SIZE, fp) != NULL) {
        if (buffer[0] == 'v' && buffer[1] == ' ') {
            vertsCount++;
        }

        if (buffer[0] == 'v' && buffer[1] == 'n' && buffer[2] == ' ') {
            normalCount++;
        }

        if (buffer[0] == 'v' && buffer[1] == 't' && buffer[2] == ' ') {
            uvsCount++;
        }

        if (buffer[0] == 'f' && buffer[1] == ' ') {
            trisCount++;

            int triCheck = 0;
            for (int i = 2; i < BUFFER_SIZE; i++) {
                if (buffer[i+1] == '\r' || 
                    buffer[i+1] == '\n' || 
                    buffer[i+1] == '\0' || 
                    buffer[i] == '\n' || 
                    buffer[i] == '\0') {
                    break;
                }

                if ( buffer[i] == ' ' ) triCheck++;

                if ( triCheck > 2 && buffer[i] == ' ' ) trisCount++;
            }
        }
    }

    if (loadDefatulTex && uvsCount > 0 && pathTexture == NULL ) {
        puts("Loading default texture");
        texture = load_default_texture();
    } 

    if (uvsCount > 0 && pathTexture != NULL ) {
        printf("Loading %s texture\n", pathTexture);
    }

    printf("tris: %ld verts: %ld normals: %ld uvs: %ld \n", trisCount, vertsCount, normalCount, uvsCount);

    Triangle *tris = malloc(sizeof(Triangle) * trisCount);
    Vec3 *verts = malloc(sizeof(Vec3) * vertsCount);
    Vec3 *normals = malloc(sizeof(Vec3) * normalCount);
    Vec3 *uvs = malloc(sizeof(Vec3) * uvsCount);

    rewind(fp);

    Triangle tempTri = {0};
    tempTri.color = GRAY;

    size_t vertsIndex = 0;
    size_t normalIndex = 0;
    size_t uvsIndex = 0;
    size_t trisIndex = 0;

    char buf[BUFFER_SIZE*3];

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
                if (flipNormals) normals[normalIndex] = vec3_multiply(normals[normalIndex], -1);
                normalIndex++;
            }
        }

        if (buffer[0] == 'v' && buffer[1] == 't' && buffer[2] == ' ') {
            if (uvsIndex < uvsCount) {
                uvs[uvsIndex] = load_vec3(buffer, 3);
                uvsIndex++;
            }
        }

        if (buffer[0] == 'f' && buffer[1] == ' ') {
            int cursor = 0;
            int bufIndex = 0;
            int indexPos = 0;
            for(int i = 2; i < BUFFER_SIZE; i++) {
                if (buffer[i] == ' ' || buffer[i] == '\n' || buffer[i] == '\0') {
                    buf[(cursor % 3) * BUFFER_SIZE + bufIndex ] = '\0';

                    switch(indexPos) {
                    case 0:
                        tempTri.v1 = atol(&buf[0]);
                        tempTri.t1 = atol(&buf[1*BUFFER_SIZE]);
                        tempTri.n1 = atol(&buf[2*BUFFER_SIZE]);
                        tempTri.v1 += -1;
                        tempTri.t1 += -1;
                        tempTri.n1 += -1;
                        break;
                    case 1:
                        tempTri.v2 = atol(&buf[0]);
                        tempTri.t2 = atol(&buf[1*BUFFER_SIZE]);
                        tempTri.n2 = atol(&buf[2*BUFFER_SIZE]);
                        tempTri.v2 += -1;
                        tempTri.t2 += -1;
                        tempTri.n2 += -1;
                        break;
                    case 2:
                        tempTri.v3 = atol(&buf[0]);
                        tempTri.t3 = atol(&buf[1*BUFFER_SIZE]);
                        tempTri.n3 = atol(&buf[2*BUFFER_SIZE]);
                        tempTri.v3 += -1;
                        tempTri.t3 += -1;
                        tempTri.n3 += -1;
                        if (trisIndex < trisCount) {
                            tris[trisIndex] = tempTri;
                            trisIndex++;
                        }
                        break;
                    default:
                        tempTri.v2 = tempTri.v3;
                        tempTri.t2 = tempTri.t3;
                        tempTri.n2 = tempTri.n3;

                        tempTri.v3 = atol(&buf[0]);
                        tempTri.t3 = atol(&buf[1*BUFFER_SIZE]);
                        tempTri.n3 = atol(&buf[2*BUFFER_SIZE]);
                        tempTri.v3 += -1;
                        tempTri.t3 += -1;
                        tempTri.n3 += -1;

                        if (trisIndex < trisCount) {
                            tris[trisIndex] = tempTri;
                            trisIndex++;
                        }
                        break;
                    }

                    if (buffer[i+1] == '\r' || 
                        buffer[i+1] == '\n' || 
                        buffer[i+1] == '\0' || 
                        buffer[i] == '\n' || 
                        buffer[i] == '\0') {
                        break;
                    }

                    cursor++;
                    bufIndex = 0;
                    indexPos++;
                    continue;
                }

                if (buffer[i] == '/') {
                    buf[(cursor % 3) * BUFFER_SIZE + bufIndex ] = '\0';
                    bufIndex = 0;
                    cursor++;
                    continue;
                }
                buf[(cursor % 3) * BUFFER_SIZE + bufIndex ] = buffer[i];
                bufIndex++;
            }
        }
    }

    if (reorder) {
        for(size_t i = 0; i < trisCount; i++) {
            size_t tempV = tris[i].v1;
            size_t tempT = tris[i].t1;
            size_t tempN = tris[i].n1;
            tris[i].v1 = tris[i].v3;
            tris[i].t1 = tris[i].t3;
            tris[i].n1 = tris[i].n3;
            tris[i].v3 = tempV;
            tris[i].t3 = tempT;
            tris[i].n3 = tempN;
        }
    }

    model = init_model(
        verts, vertsCount, 
        tris, trisCount, 
        normals, normalCount,
        uvs, uvsCount,
        texture
    );

    fclose(fp);
    free(tris);
    free(verts);
    free(normals);
    free(uvs);

    return model;
}

ModelData init_model(
    Vec3 *vert, size_t vertsCount, 
    Triangle *tri, size_t trisCount, 
    Vec3 *normals, size_t normalsCount,
    Vec3 *uvs, size_t uvsCount,
    TextureData *texture
) {
    ModelData model;

    model.texture = texture;

    model.trisCount = trisCount;
    model.vertsCount = vertsCount;
    model.normalsCount = normalsCount;
    model.uvsCount = uvsCount;

    model.verts = malloc(sizeof(Vec3) * vertsCount);
    model.tris = malloc(sizeof(Triangle) * trisCount);
    model.normals = NULL;
    model.uvs = NULL;

    if (normals != NULL) {
        model.normals = malloc(sizeof(Vec3) * normalsCount);
        for ( size_t i = 0; i < normalsCount; i++) {
            model.normals[i] = normals[i];
        }
    }

    if (uvs != NULL) {
        model.uvs = malloc(sizeof(Vec3) * uvsCount);
        for ( size_t i = 0; i < uvsCount; i++) {
            model.uvs[i] = uvs[i];
        }
    }

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

    if (model->normals != NULL) {
        free(model->normals);
    }

    if (model->uvs != NULL) {
        free(model->uvs);
    }

    free(model->verts);
    free(model->tris);
    free_texture(model->texture);
    free(model->texture);

    model->verts = NULL;
    model->tris = NULL;
    model->normals = NULL;
    model->uvs = NULL;
    model->texture = NULL;
}

void free_texture(TextureData *texture) {
    if ( texture == NULL ) return;
    if ( texture->colors == NULL ) return;
    
    free(texture->colors);
    texture->colors = NULL;
}

Instance init_instance(ModelData *model, Transforms transform){
    transform.matrixTransform = init_matrix();

    Instance i = (Instance) {
        .model = model,
        .transforms = transform,
        .trisWorld = malloc(sizeof(FullTriangle) * model->trisCount),
        .trisClipped = malloc(sizeof(FullTriangle) * model->trisCount * 20),
        .trisClippedCount = 0,
        .fromObj = model->normals != NULL
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

Color texel_from_texture(TextureData *texture, float u, float v) {
    if (u > 1 || u < 0 || v > 1 || v < 0) {
        return BLACK;
    }

    uint16_t x = u * texture->width;
    uint16_t y = v * texture->height;
    size_t i = y * texture->width + x;
    if (i >= texture->width * texture->height) {
        return BLACK;
    }
    return texture->colors[i];
}