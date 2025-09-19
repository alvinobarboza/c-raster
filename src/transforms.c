#include <stddef.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "transforms.h"

float *init_matrix() {
    float *matrix = malloc(sizeof(float) * M4X4);

    if (matrix == NULL) {
        return NULL;
    }

    float identity[M4X4] = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1,
    };

    for (size_t i = 0; i < M4X4; i++) {
        matrix[i] = identity[i];
    }

    return matrix;
}

void make_scale_matrix(float *mat, Vec3 scale) {
    float tempMat[M4X4] = {
        scale.x, 0.0f,    0.0f,    0.0f,
        0.0f,    scale.y, 0.0f,    0.0f,
        0.0f,    0.0f,    scale.z, 0.0f,
        0.0f,    0.0f,    0.0f,    1.0f,
	};
    
    for (size_t i = 0; i < M4X4; i++) {
        mat[i] = tempMat[i];
    }
}

void make_rotation_matrix(float *mat, Vec3 angle) {
    float cosa = cos(angle.x * -T_DEG2RAD);
	float sina = sin(angle.x * -T_DEG2RAD);

	float cosb = cos(angle.y * -T_DEG2RAD);
	float sinb = sin(angle.y * -T_DEG2RAD);

	float cosga = cos(angle.z * -T_DEG2RAD);
	float singa = sin(angle.z * -T_DEG2RAD);

	// Formula for general 3D roation using matrix
    float tempMat[M4X4] = {
        cosb * cosga, sina*sinb*cosga - cosa*singa, cosa*sinb*cosga + sina*singa, 0.0f,
        cosb * singa, sina*sinb*singa + cosa*cosga, cosa*sinb*singa - sina*cosga, 0.0f,
        -sinb,        sina * cosb,                  cosa * cosb,                  0.0f,
        0.0f,         0.0f,                         0.0f,                         1.0f,
	};
    
    for (size_t i = 0; i < M4X4; i++) {
        mat[i] = tempMat[i];
    }
}


void make_translation_matrix(float *mat, Vec3 position) {
    float tempMat[M4X4] = {
        1.0f,   0.0f,   0.0f,   position.x,
        0.0f,   1.0f,   0.0f,   position.y,
        0.0f,   0.0f,   1.0f,   position.z,
        0.0f,   0.0f,   0.0f,   1.0f,
	};
    
    for (size_t i = 0; i < M4X4; i++) {
        mat[i] = tempMat[i];
    }
}

Vec3 mult_matrix_by_vec3(float *mat, Vec3 v) {
    float v4[MAT_LENGTH] = {v.x, v.y, v.z, 1.0f};
    float result[MAT_LENGTH] = {0.0f, 0.0f, 0.0f, 0.0f};

    for (size_t row = 0; row < MAT_LENGTH; row++) {
        for (size_t col = 0; col < MAT_LENGTH; col++) {
            result[row] += v4[col] * mat[row*MAT_LENGTH+col];
        }            
    }        
    
    return (Vec3) {.x = result[0], .y = result[1], .z = result[2]};
}

void matrix_multiplication(float *mat_a, float *mat_b, float *result) {
    for (size_t row = 0; row < MAT_LENGTH; row++) {
        for (size_t col = 0; col < MAT_LENGTH; col++) {
            result[row*MAT_LENGTH+col] = 0.0f;
            for (size_t k = 0; k < MAT_LENGTH; k++) {
                result[row*MAT_LENGTH+col] += mat_a[row*MAT_LENGTH+k] * mat_b[k*MAT_LENGTH+col];
            }
        }
    }
}

void matrix_transpose(float *mat, float *transposed) {
    for (size_t row = 0; row < MAT_LENGTH; row++) {
        for (size_t col = 0; col < MAT_LENGTH; col++) {
            transposed[col*MAT_LENGTH+row] = mat[row*MAT_LENGTH+col];
        }
    }    
}

void print_mat(const char *name,float *mat) {
    printf("=====<%s>=======\n", name);
    for (size_t row = 0; row < MAT_LENGTH; row++)
    {
        for (size_t col = 0; col < MAT_LENGTH; col++)
        {
            printf("%+02.02f ", mat[row*MAT_LENGTH+col]);
        }
        puts("");
    }
    puts("===============");
}

void print_vec3(const char *name, Vec3 v) {
    printf("%s { .x: %+02.02f, .y: %+02.02f, z: %+02.02f }\n", name, v.x, v.y, v.z);
}

float vec3_by_vec3_multiply(Vec3 v1, Vec3 v2) {
    return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}

float vec3_dot(Vec3 v) {
    return vec3_by_vec3_multiply(v, v);
}

float vec3_length(Vec3 v) {
    return sqrtf(vec3_dot(v));
}


Vec3 vec3_multiply(Vec3 v, float n) {
    return (Vec3) {
        .x = v.x * n,
        .y = v.y * n,
        .z = v.z * n
    };
}

Vec3 vec3_normal(Vec3 v) {
    float n = vec3_length(v);
    v.x = v.x / n;
    v.y = v.y / n;
    v.z = v.z / n;
    return v;
}

Vec3 vec3_cross(Vec3 v1, Vec3 v2) {
    return (Vec3){
		.x = v1.y*v2.z - v1.z*v2.y,
		.y = v1.z*v2.x - v1.x*v2.z,
		.z = v1.x*v2.y - v1.y*v2.x,
	};
}

Vec3 vec3_add(Vec3 v1, Vec3 v2) {
    return (Vec3) {
        .x = v1.x + v2.x,
        .y = v1.y + v2.y,
        .z = v1.z + v2.z,
    };
}