#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "input.h"
#include "math/mat4.h"
#include "math/vec3.h"
#include "meshes/quad.h"

#define FAIL(message, ...) { fprintf(stderr, (message), ##__VA_ARGS__); exit_code = EXIT_FAILURE; goto terminate; }
#define PI 3.14159

int exit_code = EXIT_SUCCESS;
bool running = true;

void handle_interrupt(int _) {
	if (!running)
		exit(EXIT_FAILURE);
	running = false;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
	glViewport(0, 0, width, height);
}

int main() {
	signal(SIGINT, handle_interrupt);

	if (glfwInit() == GLFW_FALSE)
		FAIL("Failed to initialize GLFW\n");

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow *window = glfwCreateWindow(800, 600, "Untitled FPS", NULL, NULL);
	if (window == NULL)
		FAIL("Failed to create GLFW window\n");

	glfwMakeContextCurrent(window);
	if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress))
		FAIL("Failed to initialize GLAD\n");

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, glfw_key_callback);
	glfwSetCursorPosCallback(window, glfw_cursor_pos_callback);

	Vec3 pos = (Vec3){ 0, 0, 0 };
	input_set_keybind(GLFW_KEY_W, INPUT_MOVE_FORWARD);
	input_set_keybind(GLFW_KEY_A, INPUT_MOVE_LEFT);
	input_set_keybind(GLFW_KEY_S, INPUT_MOVE_BACKWARD);
	input_set_keybind(GLFW_KEY_D, INPUT_MOVE_RIGHT);

	glViewport(0, 0, 800, 600);
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

	quad_setup();

	double _time = glfwGetTime();
	while(!glfwWindowShouldClose(window) && running) {
		glfwSwapBuffers(window);
		glfwPollEvents();

		double time = glfwGetTime();
		float delta = time - _time;
		_time = time;

		pos = vec3_add(pos, vec3_mul(vec3_normalize((Vec3){
			input_check_action(INPUT_MOVE_RIGHT) - input_check_action(INPUT_MOVE_LEFT),
			0.0,
			input_check_action(INPUT_MOVE_BACKWARD) - input_check_action(INPUT_MOVE_FORWARD),
		}), 2.0 * delta));

		Mat4 mvp = mat4_identity();
		mvp = mat4_multiply(mvp, mat4_perspective(PI/2, 1.33, 0.01, 10.0));
		mvp = mat4_multiply(mvp, mat4_look_at(pos, (Vec3){ pos.x, pos.y, pos.z - 1.0 }, (Vec3){ 0, 1, 0 }));

		glClear(GL_COLOR_BUFFER_BIT);

		Mat4 m = mat4_identity();
		m = mat4_multiply(m, mat4_rotate((Vec3){ 0, 1, 0 }, PI/4));
		m = mat4_multiply(m, mat4_rotate((Vec3){ 1, 0, 0 }, PI/2));
		for (float y = -0.5; y <= 0.5; y += 1.0)
			for (float x = -4.0; x <= 4.0; x += 1.0)
				for (float z = -4.0; z <= 4.0; z += 1.0)
					quad_draw(mat4_multiply(mvp, mat4_multiply(mat4_translate((Vec3){ x, y, z }), m)));
	}

terminate:
	glfwTerminate();
	return exit_code;
}
