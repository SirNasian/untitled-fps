#ifndef CLIENT_MESH_H
#define CLIENT_MESH_H

#include <stddef.h>

#include <glad/gl.h>

typedef struct {
	GLuint vao;
	GLenum draw_mode;
	GLsizei draw_count;
} Mesh;

Mesh mesh_create(GLenum draw_mode, GLfloat *vertices, size_t vertices_size, GLuint *indices, GLsizei indices_count);
void mesh_draw(Mesh mesh);

#endif
