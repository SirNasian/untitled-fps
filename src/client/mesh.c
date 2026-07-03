#include "mesh.h"

Mesh mesh_create(GLenum draw_mode, GLfloat *vertices, size_t vertices_size, GLuint *indices, GLsizei indices_count) {
	Mesh mesh;
	mesh.draw_mode = draw_mode;
	mesh.draw_count = indices_count;

	GLuint vbo, ebo;

	glGenVertexArrays(1, &mesh.vao);
	glBindVertexArray(mesh.vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices_size, vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices_count, indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	return mesh;
}

void mesh_draw(Mesh mesh) {
	glBindVertexArray(mesh.vao);
	glDrawElements(mesh.draw_mode, mesh.draw_count, GL_UNSIGNED_INT, 0);
}
