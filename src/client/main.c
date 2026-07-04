#include <math.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "../common/math/mat4.h"
#include "../common/math/vec3.h"

#include "input.h"
#include "meshes/quad.h"

#define FAIL(message, ...) { fprintf(stderr, (message), ##__VA_ARGS__); exit_code = EXIT_FAILURE; goto terminate; }
#define PI 3.14159
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define CLAMP(value, min, max) (MAX((min), MIN((max), (value))))

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

void update_view_angles(float *yaw, float *pitch) {
	static float sensitivity = 0.001;
	float x, y;
	input_get_mouse_delta(&x, &y);
	*yaw += x * sensitivity;
	*pitch = CLAMP((*pitch) - (y * sensitivity), -1.5, 1.5);
}

Vec3 get_forward(float yaw, float pitch) {
	return (Vec3){
		sinf(yaw) * cosf(pitch),
		sinf(pitch),
		-cosf(yaw) * cosf(pitch)
	};
}

Vec3 get_move_input_direction(float yaw, float pitch) {
	Vec3 right = { cosf(yaw), 0, sinf(yaw) };
	Vec3 forward = get_forward(yaw, pitch);
	forward.y = 0;

	float input_forward = input_check_action(INPUT_MOVE_FORWARD) - input_check_action(INPUT_MOVE_BACKWARD);
	float input_right = input_check_action(INPUT_MOVE_RIGHT) - input_check_action(INPUT_MOVE_LEFT);
	Vec3 direction = vec3_add(vec3_mul(forward, input_forward), vec3_mul(right, input_right));

	return vec3_normalize(direction);
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
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	input_set_keybind(GLFW_KEY_W, INPUT_MOVE_FORWARD);
	input_set_keybind(GLFW_KEY_A, INPUT_MOVE_LEFT);
	input_set_keybind(GLFW_KEY_S, INPUT_MOVE_BACKWARD);
	input_set_keybind(GLFW_KEY_D, INPUT_MOVE_RIGHT);

	glViewport(0, 0, 800, 600);
	glClearColor(0.2, 0.3, 0.3, 1.0);

	quad_setup();

	float yaw, pitch;
	Vec3 pos = (Vec3){ 0, 0, 0 };
	double _time = glfwGetTime();
	while(!glfwWindowShouldClose(window) && running) {
		glfwSwapBuffers(window);
		glfwPollEvents();

		double time = glfwGetTime();
		float delta = time - _time;
		_time = time;

		update_view_angles(&yaw, &pitch);
		pos = vec3_add(pos, vec3_mul(get_move_input_direction(yaw, pitch), 2.0 * delta));

		Mat4 mvp = mat4_identity();
		mvp = mat4_multiply(mvp, mat4_perspective(PI/2, 1.33, 0.01, 10.0));
		mvp = mat4_multiply(mvp, mat4_look_at(pos, vec3_add(pos, get_forward(yaw, pitch)), (Vec3){ 0, 1, 0 }));

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
