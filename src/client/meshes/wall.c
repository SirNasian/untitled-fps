#include "wall.h"

#include "../mesh.h"

static Mesh mesh;

static GLfloat vertices[] = {
	 0.5f, -0.5f,  0.5f, // 0: front-right-bottom
	-0.5f, -0.5f,  0.5f, // 1: front-left-bottom
	-0.5f,  0.5f,  0.5f, // 2: front-left-top
	 0.5f,  0.5f,  0.5f, // 3: front-right-top
	 0.5f, -0.5f, -0.5f, // 4: back-right-bottom
	-0.5f, -0.5f, -0.5f, // 5: back-left-bottom
	-0.5f,  0.5f, -0.5f, // 6: back-left-top
	 0.5f,  0.5f, -0.5f, // 7: back-right-top
};

static GLuint indices[] = {
	1, 0, 3,  3, 2, 1, // front
	4, 5, 6,  6, 7, 4, // back
	0, 4, 7,  7, 3, 0, // right
	1, 2, 6,  6, 5, 1, // left
};

void wall_setup() {
	mesh = mesh_create(GL_TRIANGLES, vertices, sizeof(vertices), indices, 24);
}

void wall_draw() {
	mesh_draw(mesh);
}
