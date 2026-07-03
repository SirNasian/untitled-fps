#include "vec3.h"

#include <math.h>

Vec3 vec3_add(Vec3 a, Vec3 b) {
	return (Vec3){ (a.x + b.x), (a.y + b.y), (a.z + b.z) };
}

Vec3 vec3_sub(Vec3 a, Vec3 b) {
	return (Vec3){ (a.x - b.x), (a.y - b.y), (a.z - b.z) };
}

Vec3 vec3_mul(Vec3 v, float s) {
	return (Vec3){ (v.x * s), (v.y * s), (v.z * s) };
}

Vec3 vec3_div(Vec3 v, float s) {
	return (Vec3){ (v.x / s), (v.y / s), (v.z / s) };
}

Vec3 vec3_cross(Vec3 a, Vec3 b) {
	return (Vec3){
		(a.y * b.z) - (a.z * b.y),
		(a.z * b.x) - (a.x * b.z),
		(a.x * b.y) - (a.y * b.x),
	};
}

float vec3_dot(Vec3 a, Vec3 b) {
	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

Vec3 vec3_normalize(Vec3 v) {
	v = vec3_div(v, sqrtf(vec3_dot(v, v)));
	return (isnan(v.x)) ? (Vec3){ 0, 0, 0 } : v;
}
