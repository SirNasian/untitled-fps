#ifndef CLIENT_INPUT_H
#define CLIENT_INPUT_H

#include <stdbool.h>

#include <GLFW/glfw3.h>

typedef enum {
	INPUT_NONE,
	INPUT_MOVE_FORWARD,
	INPUT_MOVE_BACKWARD,
	INPUT_MOVE_LEFT,
	INPUT_MOVE_RIGHT,
	INPUT_LAST,
} InputAction;

void glfw_key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
void glfw_cursor_pos_callback(GLFWwindow *window, double x, double y);

void input_set_keybind(int key, InputAction action);
void input_unset_keybind(int key);

bool input_check_action(InputAction action);
void input_get_mouse_delta(float *x, float *y);

#endif
