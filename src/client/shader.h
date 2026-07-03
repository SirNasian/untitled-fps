#ifndef CLIENT_SHADER_H
#define CLIENT_SHADER_H

#include <glad/gl.h>

typedef struct {
	GLuint program;
} Shader;

Shader shader_create(const char *vertex_shader_path, const char *fragment_shader_path);
void shader_use(Shader shader);

#endif
