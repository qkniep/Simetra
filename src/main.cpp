#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.hpp"
#include "world.hpp"


void keyCallback(GLFWwindow* window, int key, int scan, int action, int mods);
void updateControls();

bool controls[6] = {false, false, false, false, false, false};
double lastFrameTime = 0;
//float camX = Chunk::chunkSizeX / 2, camY = 100, camZ = Chunk::chunkSizeZ + 1;
float camX = Chunk::chunkSizeX / 2, camY = 100, camZ = -30;
float camSpeed = 15;

World world(100);


int main() {
	static bool macMoved = false;  // part of TEMPORARY WORKAROUND

	//TODO: write wrapper around window
	GLFWwindow* window = nullptr;
	GLuint mvp_loc, shader;

	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return 1;
	}

	//using 3.2 here automatically loads newest available version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(640, 480, "Artemis", nullptr, nullptr);
	if (!window) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return 1;
	}
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, keyCallback);

	glewExperimental = GL_TRUE;  // prevent SEGFAULT
	glewInit();                  // on macOS

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	world.load();

	shader = loadShaders("shaders/vert.glsl", "shaders/frag.glsl");
	mvp_loc = glGetUniformLocation(shader, "MVP");

	while (!glfwWindowShouldClose(window)) {
		float ratio;
		int width, height;
		glm::mat4 m, v, p, mvp;
		glfwGetFramebufferSize(window, &width, &height);
		ratio = width / (float) height;
		glViewport(0, 0, width, height);

		glClearColor(0.074f, 0.058f, 0.250f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shader);

		m = glm::mat4(1);
		v = glm::lookAtLH(
			glm::vec3(camX, camY, camZ),               // camera position
			glm::vec3(camX, camY - .75f, camZ + .3f),  // position to look at
			//glm::vec3(camX, camY - .9f, camZ + .1f),         // position to look at
			glm::vec3(0, 1, 0)                         // up direction
		);
		p = glm::perspectiveLH(glm::radians(45.f), ratio, .1f, 512.f);
		mvp = p * v * m;
		glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, glm::value_ptr(mvp));

		world.render(mvp_loc, mvp);

		glfwPollEvents();
		updateControls();
		glfwSwapBuffers(window);

		//TEMPORARY WORKAROUND for not rendering before window was changed
		if (!macMoved) {
			int x, y;
			glfwGetWindowPos(window, &x, &y);
			glfwSetWindowPos(window, ++x, y);
			macMoved = true;
		}
		//END TEMP WORKAROUND
	}

	glfwTerminate();

	return 0;
}


void updateControls() {
	double currentTime = glfwGetTime();
	double deltaTime = currentTime - lastFrameTime;
	//if (controls[0]) {
		float oldCamZ = camZ;
		camZ += camSpeed * deltaTime;
		if ((int) oldCamZ % Chunk::chunkSizeZ == Chunk::chunkSizeZ - 1 && (int) camZ % Chunk::chunkSizeZ == 0) {
			world.loadNextRow();
		}
	//}
	if (controls[1])
		camZ -= camSpeed * deltaTime;
	if (controls[2])
		camX += camSpeed * deltaTime;
	if (controls[3])
		camX -= camSpeed * deltaTime;
	if (controls[4])
		camY += camSpeed * deltaTime;
	if (controls[5])
		camY -= camSpeed * deltaTime;
	lastFrameTime = currentTime;
}


void keyCallback(GLFWwindow* /*window*/, int key, int /*scan*/, int action, int /*mods*/) {
	switch (key) {
		case GLFW_KEY_W:
		case GLFW_KEY_UP:
			controls[0] = (action != GLFW_RELEASE);
			break;
		case GLFW_KEY_S:
		case GLFW_KEY_DOWN:
			controls[1] = (action != GLFW_RELEASE);
			break;
		case GLFW_KEY_D:
		case GLFW_KEY_RIGHT:
			controls[2] = (action != GLFW_RELEASE);
			break;
		case GLFW_KEY_A:
		case GLFW_KEY_LEFT:
			controls[3] = (action != GLFW_RELEASE);
			break;
		case GLFW_KEY_SPACE:
			controls[4] = (action != GLFW_RELEASE);
			break;
		case GLFW_KEY_C:
			controls[5] = (action != GLFW_RELEASE);
			break;
	}
}
