#ifndef _TRANSFORMS_H
#define _TRANSFORMS_H

#define MAT_LENGTH 4
#define M4X4 MAT_LENGTH*MAT_LENGTH
#define T_PI 3.14159265358979323846f
#define T_DEG2RAD (T_PI/180.0f)
#define T_RAD2DEG (180.0f/T_PI)

typedef struct Vec3 {
    float x, y, z;
} Vec3;

typedef struct Vec4 {
    float x, y, z, w;
} Vec4;

// Must deallocate after use
float *init_matrix(); 

// Compute scale, rotation and translation matrix (XYZ)
// This order scale > rotation > translation(No effect since it is the fourth element)
void compute_matrix(float *mat, Vec3 scale, Vec3 angle, Vec3 translation);

// Multiply given vector by a arbitrary matrix
Vec3 mult_matrix_by_vec3(float *mat, Vec3 v);

// Matrix by matrix multiplication and store in "result"
void matrix_multiplication(float *mat_a, float *mat_b, float *result);

#endif