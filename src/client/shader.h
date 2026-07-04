#ifndef CLIENT_SHADER_H
#define CLIENT_SHADER_H

#include <stdbool.h>

#include <glad/gl.h>

#include "../common/math/mat4.h"

typedef struct {
	GLuint program;
} Shader;

Shader shader_create(const char *vertex_shader_path, const char *fragment_shader_path);
void shader_use(Shader shader);
void shader_set_bool(Shader shader, const char *uniform, bool value);
void shader_set_mat4(Shader shader, const char *uniform, Mat4 mat4);
void shader_set_vec3(Shader shader, const char *uniform, Vec3 vec3);

#endif
