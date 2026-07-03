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
		float delta = _time - time;
		_time = time;

		pos = vec3_add(pos, vec3_mul(vec3_normalize((Vec3){
			input_check_action(INPUT_MOVE_RIGHT) - input_check_action(INPUT_MOVE_LEFT),
			input_check_action(INPUT_MOVE_FORWARD) - input_check_action(INPUT_MOVE_BACKWARD),
			0.0,
		}), delta));

		glClear(GL_COLOR_BUFFER_BIT);

		Mat4 m = mat4_rotate((Vec3){ 0, 0, 1 }, 3.14159/4);
		m = mat4_multiply(mat4_translate((Vec3){ -pos.x, -pos.y, -pos.z }), m);

		quad_draw(mat4_multiply(mat4_translate((Vec3){ -0.5,  0.5, 0 }), m));
		quad_draw(mat4_multiply(mat4_translate((Vec3){  0.5, -0.5, 0 }), m));
		quad_draw(mat4_multiply(mat4_translate((Vec3){ -0.5, -0.5, 0 }), m));
		quad_draw(mat4_multiply(mat4_translate((Vec3){  0.5,  0.5, 0 }), m));
	}

terminate:
	glfwTerminate();
	return exit_code;
}
