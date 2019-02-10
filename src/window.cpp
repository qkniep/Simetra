#include "window.hpp"

#include <iostream>


Window::Window() {
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		exit(EXIT_FAILURE);
	}

	//using 3.2 here automatically loads newest available version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);
	window = glfwCreateWindow(mode->width, mode->height, "Simetra", monitor, nullptr);
	if (!window) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);

	glewInit();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
}


Window::~Window() {
	glfwTerminate();
}


bool Window::alive() {
	return !glfwWindowShouldClose(window);
}


void Window::update() {
	glfwPollEvents();
	glfwSwapBuffers(window);
}


void Window::setKeyCallback(GLFWkeyfun cb) {
	glfwSetKeyCallback(window, cb);
}


void Window::setSizeCallback(GLFWframebuffersizefun cb) {
	glfwSetFramebufferSizeCallback(window, cb);
}
