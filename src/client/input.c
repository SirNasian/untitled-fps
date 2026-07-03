#include "input.h"
#include "GLFW/glfw3.h"

typedef struct {
	double x;
	double y;
} Point;

InputAction _keybinds[GLFW_KEY_LAST];
bool _input_state[INPUT_LAST];

void glfw_key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
	InputAction input_action = _keybinds[key];
	if (input_action != INPUT_NONE)
		_input_state[input_action] = action != GLFW_RELEASE;
}

static Point _cursor;
static Point _cursor_delta;

void glfw_cursor_pos_callback(GLFWwindow *window, double x, double y) {
	_cursor_delta.x += x - _cursor.x;
	_cursor_delta.y += y - _cursor.y;
	_cursor.x = x;
	_cursor.y = y;
}

void input_get_mouse_delta(float *x, float *y) {
	*x = (float)_cursor_delta.x;
	*y = (float)_cursor_delta.y;
	_cursor_delta.x = 0;
	_cursor_delta.y = 0;
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
