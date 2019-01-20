#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include <vector>


struct vertex {
	GLfloat x, y, z;
	GLfloat r, g, b;
};


class Chunk {
	static const int chunkSize = 32;

	int xPos, zPos;

	std::vector<vertex> vertices;
	std::vector<unsigned int> indices;
	GLuint vao, vbo, elementbuffer, mvp_loc;


public:
	Chunk();
	Chunk(int x, int z);

	void load();
	void render(GLuint mvp_loc, glm::mat4 mvp);

private:
	void generateTerrain();
};
