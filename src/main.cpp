#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera.hpp"
#include "shader.hpp"
#include "window.hpp"
#include "world.hpp"

#define AUTO_SCROLL true


void keyCallback(GLFWwindow* window, int key, int scan, int action, int mods);
void sizeCallback(GLFWwindow* window, int width, int height);
void updateControls();

struct {
	bool forward, back;
	bool left, right;
	bool up, down;
} controls;
double lastFrameTime = 0;
const float camSpeed = 30;

World world(time(0));
Camera camera = Camera(Chunk::chunkSizeX / 2, 30, 5);


int main() {
	GLuint mvp_loc;
	Window window;
	window.setKeyCallback(keyCallback);
	window.setSizeCallback(sizeCallback);

	world.load();

	ShaderProgram shader;
	shader.load("shaders/vert.glsl", "shaders/frag.glsl");
	mvp_loc = glGetUniformLocation(shader.getProgramID(), "MVP");

	while (window.alive()) {
		glClearColor(0.694f, 0.78f, 0.925f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.use();
		glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, glm::value_ptr(camera.getMVP()));
		world.render(mvp_loc, camera.getMVP());

		window.update();
		updateControls();
	}

	return 0;
}


void updateControls() {
	double currentTime = glfwGetTime();
	double deltaTime = currentTime - lastFrameTime;
	GLfloat d = camSpeed * deltaTime;
	if (controls.forward || AUTO_SCROLL) {
		GLfloat oldCamZ = camera.getZ();
		camera.move(0, 0, d);
		if ((int) oldCamZ % Chunk::chunkSizeZ == Chunk::chunkSizeZ - 1
				&& (int) camera.getZ() % Chunk::chunkSizeZ == 0) {
			world.loadNextRow();
		}
	}
	if (controls.back)
		camera.move(0, 0, -d);
	if (controls.right)
		camera.move(d, 0, 0);
	if (controls.left)
		camera.move(-d, 0, 0);
	if (controls.up)
		camera.move(0, d, 0);
	if (controls.down)
		camera.move(0, -d, 0);
	lastFrameTime = currentTime;
}


void keyCallback(GLFWwindow* /*window*/, int key, int /*scan*/, int action, int /*mods*/) {
	switch (key) {
		case GLFW_KEY_W:
		case GLFW_KEY_UP:
			controls.forward = (action != GLFW_RELEASE);
			break;
		case GLFW_KEY_S:
		case GLFW_KEY_DOWN:
			controls.back = (action != GLFW_RELEASE);
			break;
		case GLFW_KEY_D:
		case GLFW_KEY_RIGHT:
			controls.right = (action != GLFW_RELEASE);
			break;
		case GLFW_KEY_A:
		case GLFW_KEY_LEFT:
			controls.left = (action != GLFW_RELEASE);
			break;
		case GLFW_KEY_SPACE:
			controls.up = (action != GLFW_RELEASE);
			break;
		case GLFW_KEY_C:
			controls.down = (action != GLFW_RELEASE);
			break;
	}
}


void sizeCallback(GLFWwindow* /*window*/, int width, int height) {
	float ratio;
	ratio = width / (float) height;
	glViewport(0, 0, width, height);

	camera.setAspectRatio(ratio);
}
