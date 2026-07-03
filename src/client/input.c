#include "input.h"
#include "GLFW/glfw3.h"

InputAction _keybinds[GLFW_KEY_LAST];
bool _input_state[INPUT_LAST];

void glfw_key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
	InputAction input_action = _keybinds[key];
	if (input_action != INPUT_NONE)
		_input_state[input_action] = action != GLFW_RELEASE;
}

void glfw_cursor_pos_callback(GLFWwindow *window, double x, double y) {
	// TODO: implement this
}

void input_set_keybind(int key, InputAction action) {
	_keybinds[key] = action;
}

void input_unset_keybind(int key) {
	_keybinds[key] = INPUT_NONE;
}

bool input_check_action(InputAction action) {
	return _input_state[action];
}
