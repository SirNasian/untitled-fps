#include <math.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <enet/enet.h>
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "../common/math/mat4.h"
#include "../common/math/vec3.h"
#include "../common/network.h"
#include "../common/player.h"
#include "../common/time.h"

#include "input.h"
#include "meshes/player_mesh.h"
#include "meshes/quad.h"
#include "shader.h"

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
	*yaw -= x * sensitivity;
	*pitch = CLAMP((*pitch) - (y * sensitivity), -1.5, 1.5);
}

Vec3 get_forward(float yaw, float pitch) {
	return (Vec3){
		cosf(pitch) * -sinf(yaw),
		sinf(pitch),
		cosf(pitch) * -cosf(yaw),
	};
}

Vec3 get_move_input_direction(float yaw, float pitch) {
	Vec3 right = { cosf(yaw), 0, -sinf(yaw) };
	Vec3 forward = get_forward(yaw, pitch);
	forward.y = 0;

	float input_forward = input_check_action(INPUT_MOVE_FORWARD) - input_check_action(INPUT_MOVE_BACKWARD);
	float input_right = input_check_action(INPUT_MOVE_RIGHT) - input_check_action(INPUT_MOVE_LEFT);
	Vec3 direction = vec3_add(vec3_mul(forward, input_forward), vec3_mul(right, input_right));

	return vec3_normalize(direction);
}

int main(int argc, char **argv) {
	signal(SIGINT, handle_interrupt);

	ENetHost *host;
	ENetPeer *server;
	char *server_address = argc > 1 ? argv[1] : "localhost";
	if (!network_client_setup(server_address, 42069, &host, &server))
		FAIL("failed to setup network");

	if (glfwInit() == GLFW_FALSE)
		FAIL("failed to initialize GLFW\n");

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow *window = glfwCreateWindow(800, 600, "Untitled FPS", NULL, NULL);
	if (window == NULL)
		FAIL("failed to create GLFW window\n");

	glfwMakeContextCurrent(window);
	if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress))
		FAIL("failed to initialize GLAD\n");

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

	Shader shader = shader_create("shaders/entity.vert", "shaders/entity.frag");
	shader_use(shader);
	player_mesh_setup();
	quad_setup();

	double _time = glfwGetTime();
	while(!glfwWindowShouldClose(window) && running) {
		glfwSwapBuffers(window);
		glfwPollEvents();

		double time = glfwGetTime();
		float delta = time - _time;
		_time = time;

		Vec3 *pos = &player_get_ptr()->position;
		Vec3 *rot = &player_get_ptr()->rotation;

		update_view_angles(&rot->y, &rot->x);
		*pos = vec3_add(*pos, vec3_mul(get_move_input_direction(rot->y, rot->x), 2.0 * delta));

		Mat4 mvp = mat4_identity();
		mvp = mat4_multiply(mvp, mat4_perspective(PI/2, 1.33, 0.01, 10.0));
		mvp = mat4_multiply(mvp, mat4_look_at(*pos, vec3_add(*pos, get_forward(rot->y, rot->x)), (Vec3){ 0, 1, 0 }));

		glClear(GL_COLOR_BUFFER_BIT);

		Mat4 m = mat4_identity();
		m = mat4_multiply(m, mat4_scale((Vec3){ 8, 0, 8 }));
		m = mat4_multiply(m, mat4_rotate((Vec3){ 1, 0, 0 }, PI/2));
		for (float y = -0.5; y < 1.5; y += 1.0) {
			shader_set_mat4(shader, "mvp", mat4_multiply(mvp, mat4_multiply(mat4_translate((Vec3){ 0, y, -2 }), m)));
			shader_set_vec3(shader, "colour", (Vec3){ 0.6, 0.6, 0.7 });
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			quad_draw();
		}

		for (uint32_t i = 0; i < 65536; i++) {
			Player *p = player_get_ptr_all()+i;
			if (!p->active || p == player_get_ptr()) continue;
			Mat4 m = mat4_identity();
			m = mat4_multiply(m, mat4_translate(p->position));
			m = mat4_multiply(m, mat4_rotate((Vec3){ 0, 1, 0 }, p->rotation.y));
			m = mat4_multiply(m, mat4_rotate((Vec3){ 1, 0, 0 }, p->rotation.x));
			shader_set_mat4(shader, "mvp", mat4_multiply(mvp, m));
			shader_set_vec3(shader, "colour", (Vec3){ 1.0, 0.5, 0.2 });
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			player_mesh_draw();
		}

		if (time_next_tick_ns(false) == 0)
			network_client_service(host, server);
	}

	ENetEvent event;
	enet_peer_disconnect(server, 100);
	enet_host_service(host, &event, 0);

terminate:
	enet_host_destroy(host);
	enet_deinitialize();
	glfwTerminate();
	return exit_code;
}
