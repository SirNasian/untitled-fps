#include "mat4.h"

#include <math.h>

Mat4 mat4_identity() {
	return (Mat4){{
		 1, 0, 0, 0,
		 0, 1, 0, 0,
		 0, 0, 1, 0,
		 0, 0, 0, 1,
	}};
}

Mat4 mat4_multiply(Mat4 a, Mat4 b) {
	Mat4 r = {{ 0 }};
	for (int col = 0; col < 4; col++) {
		for (int row = 0; row < 4; row++) {
			for (int k = 0; k < 4; k++) {
				r.m[col * 4 + row] += a.m[k * 4 + row] * b.m[col * 4 + k];
			}
		}
	}
	return r;
}

Mat4 mat4_translate(Vec3 t) {
	return (Mat4){{
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		t.x,  t.y,  t.z,  1.0f,
	}};
}

Mat4 mat4_scale(Vec3 s) {
	return (Mat4){{
		s.x,  0.0f, 0.0f, 0.0f,
		0.0f, s.y,  0.0f, 0.0f,
		0.0f, 0.0f, s.z,  0.0f,
		0.0f, 0.0f, 0.0f, 1.0f,
	}};
}

Mat4 mat4_rotate(Vec3 axis, float angle) {
	Vec3 a = vec3_normalize(axis);
	float c = cosf(angle), s = sinf(angle), t = 1.0f - c;
	float x = a.x, y = a.y, z = a.z;

	return (Mat4){{
		t*x*x + c,   t*x*y + s*z, t*x*z - s*y, 0.0f,
		t*x*y - s*z, t*y*y + c,   t*y*z + s*x, 0.0f,
		t*x*z + s*y, t*y*z - s*x, t*z*z + c,   0.0f,
		0.0f,        0.0f,        0.0f,         1.0f,
	}};
}

Mat4 mat4_look_at(Vec3 eye, Vec3 center, Vec3 up) {
	Vec3 f = vec3_normalize(vec3_sub(center, eye));
	Vec3 s = vec3_normalize(vec3_cross(f, up));
	Vec3 u = vec3_cross(s, f);

	return (Mat4){{
		 s.x,               u.x,              -f.x,              0.0f,
		 s.y,               u.y,              -f.y,              0.0f,
		 s.z,               u.z,              -f.z,              0.0f,
		-vec3_dot(s, eye), -vec3_dot(u, eye),  vec3_dot(f, eye), 1.0f,
	}};
}

Mat4 mat4_perspective(float fovy, float aspect, float near_z, float far_z) {
	float f     = 1.0f / tanf(fovy * 0.5f);
	float range = near_z - far_z;

	return (Mat4){{
		f / aspect, 0.0f, 0.0f,                              0.0f,
		0.0f,       f,    0.0f,                              0.0f,
		0.0f,       0.0f, (far_z + near_z) / range,         -1.0f,
		0.0f,       0.0f, (2.0f * far_z * near_z) / range,   0.0f,
	}};
}
