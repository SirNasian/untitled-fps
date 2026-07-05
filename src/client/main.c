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
#include "meshes/wall.h"
#include "shader.h"

#define FAIL(message, ...) { fprintf(stderr, (message), ##__VA_ARGS__); exit_code = EXIT_FAILURE; goto terminate; }
#define PI 3.14159
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define CLAMP(value, min, max) (MAX((min), MIN((max), (value))))
#define PLAYER_RADIUS 0.1

static int exit_code = EXIT_SUCCESS;
static bool running = true;

static uint8_t map_data[256];

void handle_interrupt(int _) {
	if (!running)
		exit(EXIT_FAILURE);
	running = false;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
	glViewport(0, 0, width, height);
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

void update_view_angles(float *yaw, float *pitch) {
	static float sensitivity = 0.001;
	float x, y;
	input_get_mouse_delta(&x, &y);
	*yaw -= x * sensitivity;
	*pitch = CLAMP((*pitch) - (y * sensitivity), -1.5, 1.5);
}

bool circle_overlaps_wall(float cx, float cz) {
	int tx_min = (int)floorf(cx - PLAYER_RADIUS);
	int tx_max = (int)floorf(cx + PLAYER_RADIUS);
	int tz_min = (int)floorf(cz - PLAYER_RADIUS);
	int tz_max = (int)floorf(cz + PLAYER_RADIUS);

	for (int tz = tz_min; tz <= tz_max; tz++) {
		for (int tx = tx_min; tx <= tx_max; tx++) {
			if (tx >= 0 && tx < 16 && tz >= 0 && tz < 16 && map_data[16*tz + tx]) {
				float nx = fmaxf(tx, fminf(cx, tx + 1.0f));
				float nz = fmaxf(tz, fminf(cz, tz + 1.0f));
				float dx = cx - nx;
				float dz = cz - nz;
				if (dx*dx + dz*dz < PLAYER_RADIUS*PLAYER_RADIUS)
					return true;
			}
		}
	}
	return false;
}

void update_view_position(Vec3 *pos, const Vec3 *rot, float delta) {
	Vec3 new_pos = vec3_add(*pos, vec3_mul(get_move_input_direction(rot->y, rot->x), 2.0 * delta));
	if (!circle_overlaps_wall(pos->x, new_pos.z)) pos->z = new_pos.z;
	if (!circle_overlaps_wall(new_pos.x, pos->z)) pos->x = new_pos.x;
}

int main(int argc, const char **argv) {
	signal(SIGINT, handle_interrupt);

	ENetHost *host;
	ENetPeer *server;
	const char *server_address = argc > 1 ? argv[1] : "localhost";
	int server_port = argc > 2 ? atoi(argv[2]) : 42069;
	if (!network_client_setup(server_address, server_port, &host, &server, map_data))
		FAIL("failed to setup network");

	if (glfwInit() == GLFW_FALSE)
		FAIL("failed to initialize GLFW\n");

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

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
	glEnable(GL_DEPTH_TEST);

	Shader shader = shader_create("shaders/entity.vert", "shaders/entity.frag");
	shader_use(shader);
	shader_set_mat4(shader, "projection", mat4_perspective(PI/2, 1.33, 0.01, 32.0));

	player_mesh_setup();
	quad_setup();
	wall_setup();

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
		update_view_position(pos, rot, delta);

		Mat4 mvp = mat4_identity();
		mvp = mat4_multiply(mvp, mat4_perspective(PI/2, 1.33, 0.01, 32.0));
		mvp = mat4_multiply(mvp, mat4_look_at(*pos, vec3_add(*pos, get_forward(rot->y, rot->x)), (Vec3){ 0, 1, 0 }));

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Mat4 m;
		shader_set_mat4(shader, "view", mat4_look_at(*pos, vec3_add(*pos, get_forward(rot->y, rot->x)), (Vec3){ 0, 1, 0 }));
		shader_set_vec3(shader, "camera", *pos);

		for (int i = 0; i < 256; i++) {
			if (!map_data[i]) continue;
			m = mat4_translate((Vec3){ (i % 16) + 0.5, 0, (int)(i / 16) + 0.5 });
			shader_set_mat4(shader, "model", m);
			shader_set_vec3(shader, "colour", (Vec3){ 0.6, 0.6, 0.6 });
			shader_set_bool(shader, "fullbright", false);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			wall_draw();
		}

		for (uint32_t i = 0; i < 65536; i++) {
			Player *p = player_get_ptr_all()+i;
			if (!p->active || p == player_get_ptr())
				continue;

			m = mat4_identity();
			m = mat4_multiply(m, mat4_translate(p->position));
			m = mat4_multiply(m, mat4_rotate((Vec3){ 0, 1, 0 }, p->rotation.y));
			m = mat4_multiply(m, mat4_rotate((Vec3){ 1, 0, 0 }, p->rotation.x));
			shader_set_mat4(shader, "model", m);
			shader_set_vec3(shader, "colour", (Vec3){ 1.0, 0.5, 0.2 });
			shader_set_bool(shader, "fullbright", true);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			player_mesh_draw();

			m = mat4_identity();
			m = mat4_multiply(m, mat4_translate(p->position));
			m = mat4_multiply(m, mat4_scale((Vec3){ 0.2, 0.2, 0.2 }));
			shader_set_mat4(shader, "model", m);
			wall_draw();
		}

		Vec3 billboard_pos = { 8, 0, 4.5 };
		Vec3 to_player = vec3_sub(*pos, billboard_pos);
		m = mat4_identity();
		m = mat4_multiply(m, mat4_translate(billboard_pos));
		m = mat4_multiply(m, mat4_rotate((Vec3){ 0, 1, 0 }, atan2f(to_player.x, to_player.z)));
		m = mat4_multiply(m, mat4_scale((Vec3){ 0.5, 0.5, 0.5 }));
		shader_set_mat4(shader, "model", m);
		shader_set_vec3(shader, "colour", (Vec3){ 1, 0, 0 });
		shader_set_bool(shader, "fullbright", true);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		quad_draw();

		if (time_next_tick_ns(false) == 0)
			network_client_service(host, server);
	}

terminate:
	network_client_teardown(host);
	glfwTerminate();
	return exit_code;
}
