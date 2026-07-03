#include "quad.h"

#include "../mesh.h"
#include "../shader.h"

Shader shader;
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
	shader = shader_create("shaders/triangle.vert", "shaders/triangle.frag");
	mesh = mesh_create(GL_TRIANGLES, vertices, sizeof(vertices), indices, 6);
}

void quad_draw() {
	shader_use(shader);
	mesh_draw(mesh);
}
