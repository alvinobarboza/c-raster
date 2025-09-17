#include <stddef.h>
#include <math.h>
#include <stdlib.h>

#include "transforms.h"

float *init_matrix() {
    float *matrix = malloc(sizeof(float) * M4X4);

    if (matrix == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < M4X4; i++) {
        matrix[i] = 0.0f;
    }

    return matrix;
}

void compute_matrix(float *mat, Vec3 scale, Vec3 angle, Vec3 translation) {
    float cosa = cos(angle.x * -T_DEG2RAD);
	float sina = sin(angle.x * -T_DEG2RAD);

	float cosb = cos(angle.y * -T_DEG2RAD);
	float sinb = sin(angle.y * -T_DEG2RAD);

	float cosga = cos(angle.z * -T_DEG2RAD);
	float singa = sin(angle.z * -T_DEG2RAD);

	// Formula for general 3D roation using matrix
    float tempMat[M4X4] = {
    scale.x * (cosb * cosga), sina*sinb*cosga - cosa*singa,             cosa*sinb*cosga + sina*singa, translation.x,
    cosb * singa,             scale.y * (sina*sinb*singa + cosa*cosga), cosa*sinb*singa - sina*cosga, translation.y,
    -sinb,                    sina * cosb,                              scale.z * (cosa * cosb),      translation.z,
    0.0f,                     0.0f,                                     0.0f,                         1.0f,
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