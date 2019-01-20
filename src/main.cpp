#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "FastNoise/FastNoise.h"

#include <cmath>
#include <map>
#include <vector>

#include "shader.hpp"

#define X 0.52573111212
#define Z 0.85065080835


void key_callback(GLFWwindow* window, int key, int scan, int action, int mods);
void updateControls();

bool controls[6] = {false, false, false, false, false, false};
double lastFrameTime = 0;
float camX = 0, camY = 0, camZ = 0;
float camSpeed = 15;


struct vertex {
	GLfloat x, y, z;
	GLfloat r, g, b;
};

std::vector<vertex> vertices;
std::vector<unsigned int> indices;

GLuint vao, vbo, elementbuffer;


#define SIZE 128


void generatePlane() {
	FastNoise noise = FastNoise(47);

	for (int x = 0; x < SIZE+1; ++x) {
		for (int z = 0; z < SIZE+1; ++z) {
			float fx = x, fz = z;
			//int biome = (int) round(1.5f * (noise.GetCellular(x, z) + 1));
			float height = noise.GetSimplex(x, z)
				+ .5f  * noise.GetSimplex(2 * x, 2 * z)
				+ .25f * noise.GetSimplex(4 * x, 4 * z);

			//if (biome < 3) height = -1.75;

			float colorR = 0, colorG = 0, colorB = 0;
			if (height < -0.5f) {
				float r = rand() % 24 / 256.f;
				colorR = 0.407f * (1 + r);
				colorG = 0.427f * (1 + r);
				colorB = 0.878f * (1 + r);
			} else if (height < 0.6f) {
				float r = rand() % 24 / 256.f;
				colorR = 0.4140625f * (1 + r);
				colorG = 0.6875f * (1 + r);
				colorB = 0.296875f * (1 + r);
			} else if (height < 0.9) {
				height += .5f * noise.GetSimplex(8 * x, 8 * z);
				int r = rand() % 32;
				colorR = (160 + r) / 256.f;
				colorG = (160 + r) / 256.f;
				colorB = (160 + r) / 256.f;
			} else {
				height += .5f * noise.GetSimplex(8 * x, 8 * z);
				colorR = (230 + rand() % 16) / 256.f;
				colorG = (230 + rand() % 16) / 256.f;
				colorB = 1;
			}

			height += 1.75;
			height *= 0.4f;
			height = powf(height, 4.6);
			height *= 10;

			vertices.push_back({fx, height, -fz, colorR, colorG, colorB});
		}
	}

	for (int x = 0; x < SIZE; ++x) {
		for (int z = 0; z < SIZE; ++z) {
			//float fx = x, fz = z;
			//float height = 5 * noise.GetSimplex(x, z);
			//vertices.push_back({fx, height, fz, height / 10.f + .5f, 0, 0});

			unsigned int v = x * (SIZE + 1) + z;
			unsigned int quad[] = {
				v, v + SIZE + 1, v + SIZE + 2,
				v, v + SIZE + 2, v + 1,
			};
			indices.insert(indices.end(), quad, quad + 6);
		}
	}
}


int main() {
	generatePlane();

	static bool macMoved = false;  // part of TEMPORARY WORKAROUND

	GLFWwindow *window = NULL;
	const GLubyte *renderer, *version;
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

	window = glfwCreateWindow(640, 480, "Artemis", NULL, NULL);
	if (!window) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return 1;
	}
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);

	glewExperimental = GL_TRUE;  // prevent SEGFAULT
	glewInit();                  // on macOS

	renderer = glGetString(GL_RENDERER);
	version = glGetString(GL_VERSION);
	printf("Renderer: %s\n", renderer);
	printf( "OpenGL version supported %s\n", version );

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(struct vertex), &vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*) 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*) (sizeof(float) * 3));

	glGenBuffers(1, &elementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	shader = LoadShaders("shaders/vert.glsl", "shaders/frag.glsl");
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
		v = glm::lookAt(
			glm::vec3(camX, camY, camZ), // camera position
			glm::vec3(camX, camY, camZ - 1), // look in front of you
			glm::vec3(0, 1, 0) // head's up
		);
		p = glm::perspective(glm::radians(45.f), ratio, .1f, 256.f);
		mvp = p * v * m;
		glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, glm::value_ptr(mvp));

		glBindVertexArray(vao);

		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (void*) 0);

		glfwPollEvents();
		updateControls();
		glfwSwapBuffers(window);

		//TEMPORARY WORKAROUND for not rendering before window was changed
		if(!macMoved) {
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
	if (controls[0])
		camZ += camSpeed * deltaTime;
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


void key_callback(GLFWwindow* window, int key, int scan, int action, int mods) {
	switch (key) {
		case GLFW_KEY_S:
		case GLFW_KEY_DOWN:
			controls[0] = (action != GLFW_RELEASE);
			break;
		case GLFW_KEY_W:
		case GLFW_KEY_UP:
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
