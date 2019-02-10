#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>


class Window {
	GLFWwindow* window = nullptr;


public:
	Window();
	~Window();
	bool alive();
	void update();
	void setKeyCallback(GLFWkeyfun cb);
	void setSizeCallback(GLFWframebuffersizefun cb);
};

#endif  // WINDOW_HPP
