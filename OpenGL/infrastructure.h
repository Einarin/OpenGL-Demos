#pragma once

void glfwErrorCallback(int error, const char* description);
void APIENTRY openglErrorCallback(GLenum source,
						 GLenum type,
						 GLuint id,
						 GLenum severity,
						 GLsizei length,
						 const GLchar* message,
						 const void* userParam);
void onKeyPressed(GLFWwindow* window, int key, int scancode, int action, int modifiers);
GLFWwindow* init(int windowWidth, int windowHeight, const char* windowTitle);