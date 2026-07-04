#include "player_mesh.h"

#include "../mesh.h"

static Mesh mesh;

static GLfloat vertices[] = {
	 0.00,  0.00,  0.0, // 0: apex (back)
	 0.30,  0.20, -0.5, // 1: top-right (front)
	-0.30,  0.20, -0.5, // 2: top-left (front)
	-0.30, -0.20, -0.5, // 3: bottom-left (front)
	 0.30, -0.20, -0.5, // 4: bottom-right (front)
};

static GLuint indices[] = {
	// sides
	0, 2, 1,
	0, 3, 2,
	0, 4, 3,
	0, 1, 4,
	// front face
	1, 2, 3,
	1, 3, 4,
};

void player_mesh_setup() {
	mesh = mesh_create(GL_TRIANGLES, vertices, sizeof(vertices), indices, 18);
}

void player_mesh_draw() {
	mesh_draw(mesh);
}
