#include "quad.h"

#include "../mesh.h"

Mesh mesh;

GLfloat vertices[] = {
	-0.5f, -0.5f, 0.0f,
	-0.5f,  0.5f, 0.0f,
	 0.5f,  0.5f, 0.0f,
	 0.5f, -0.5f, 0.0f,
};

GLuint indices[] = {
	0, 1, 2,
	2, 3, 0,
};

void quad_setup() {
	mesh = mesh_create(GL_TRIANGLES, vertices, sizeof(vertices), indices, 6);
}

void quad_draw() {
	mesh_draw(mesh);
}
