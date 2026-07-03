#include "shader.h"

#include <stdio.h>
#include <stdlib.h>

GLuint shader_load(const char *path, GLenum type) {
	FILE *file = fopen(path, "rb");
	if (!file) {
		fprintf(stderr, "shader_load: could not open '%s'\n", path);
		return 0;
	}

	fseek(file, 0, SEEK_END);
	long file_size = ftell(file);
	rewind(file);

	char *shader_source = malloc(file_size+1);
	if (!shader_source) {
		fclose(file);
		fprintf(stderr, "shader_load: failed to allocate memory for shader source for '%s'\n", path);
		return 0;
	}

	fread(shader_source, 1, file_size, file);
	shader_source[file_size] = '\0';
	fclose(file);

	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, (const char **)&shader_source, NULL);
	glCompileShader(shader);
	free(shader_source);

	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (!status) {
		char log[512];
		glGetShaderInfoLog(shader, sizeof(log), NULL, log);
		fprintf(stderr, "shader_load: compile error in '%s':\n%s\n", path, log);
		glDeleteShader(shader);
		return 0;
	}

	return shader;
}

GLuint shader_program_load(const char *vertex_shader_path, const char *fragment_shader_path) {
	GLuint vertex_shader = shader_load(vertex_shader_path, GL_VERTEX_SHADER);
	GLuint fragment_shader = shader_load(fragment_shader_path, GL_FRAGMENT_SHADER);

	GLuint program = glCreateProgram();
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	glLinkProgram(program);

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	GLint status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (!status) {
		char log[512];
		glGetProgramInfoLog(program, sizeof(log), NULL, log);
		fprintf(stderr, "shader_program_load: linking failed:\n%s\n", log);
		glDeleteProgram(program);
		return 0;
	}

	return program;
}

Shader shader_create(const char *vertex_shader_path, const char *fragment_shader_path) {
	Shader shader;
	shader.program = shader_program_load(vertex_shader_path, fragment_shader_path);
	return shader;
}

void shader_use(Shader shader) {
	glUseProgram(shader.program);
}
