#ifndef CLIENT_MATH_MAT4_H
#define CLIENT_MATH_MAT4_H

#include "vec3.h"

typedef struct {
	float m[16];
} Mat4;

Mat4 mat4_identity();
Mat4 mat4_multiply(Mat4 a, Mat4 b);

Mat4 mat4_translate(Vec3 t);
Mat4 mat4_scale(Vec3 s);
Mat4 mat4_rotate(Vec3 axis, float angle);

Mat4 mat4_look_at(Vec3 eye, Vec3 center, Vec3 up);
Mat4 mat4_perspective(float fovy, float aspect, float near_z, float far_z);

#endif
