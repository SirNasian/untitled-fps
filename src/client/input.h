#ifndef CLIENT_INPUT_H
#define CLIENT_INPUT_H

#include <GLFW/glfw3.h>

void glfw_key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
void glfw_cursor_pos_callback(GLFWwindow *window, double x, double y);

#endif
