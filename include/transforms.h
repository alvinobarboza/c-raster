#ifndef _TRANSFORMS_H
#define _TRANSFORMS_H

#define MAT_LENGTH 4
#define M4X4 MAT_LENGTH*MAT_LENGTH
#define T_PI 3.14159265358979323846f
#define T_DEG2RAD (T_PI/180.0f)
#define T_RAD2DEG (180.0f/T_PI)

typedef struct Vec2 {
    float x, y;
} Vec2;

typedef struct Vec3 {
    float x, y, z;
} Vec3;

typedef struct Vec4 {
    float x, y, z, w;
} Vec4;

typedef struct Transforms {
    Vec3 scale, rotation, position;
    float *matrixTransform;
} Transforms;

// Must deallocate after use
float *init_matrix(); 

// Transpose matrix -> useful for inverting 3d roation
void matrix_transpose(float *mat, float *transposed);

// Transformation matrices 
void make_scale_matrix(float *mat, Vec3 scale);
void make_rotation_matrix(float *mat, Vec3 angle);
void make_translation_matrix(float *mat, Vec3 position);

// Multiply given vector by a arbitrary matrix
Vec3 mult_matrix_by_vec3(float *mat, Vec3 v);

// Matrix by matrix multiplication and store in "result"
void matrix_multiplication(float *mat_a, float *mat_b, float *result);

// For debug purposes
void print_mat(const char *name, float *mat);
void print_vec3(const char *name, Vec3 v);

// Vector Maths
float vec3_by_vec3_multiply(Vec3 v1, Vec3 v2);
float vec3_dot(Vec3 v);
float vec3_length(Vec3 v);
Vec3 vec3_normal(Vec3 v);
Vec3 vec3_cross(Vec3 v1, Vec3 v2);
Vec3 vec3_multiply(Vec3 v, float n);
Vec3 vec3_divide(Vec3 v, float n);
Vec3 vec3_add(Vec3 v1, Vec3 v2);
Vec3 vec3_sub(Vec3 v1, Vec3 v2);
void vec3_swap_values(Vec3 *v1, Vec3 *v2);
Vec3 vec3_lerp_a_b(Vec3 a, Vec3 b, float ratio);

#endif