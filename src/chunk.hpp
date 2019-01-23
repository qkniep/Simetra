#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include <vector>


struct vertex {
	GLfloat x, y, z;
	GLfloat r, g, b;
};


class Chunk {
public:
	static const int chunkSize = 32;

	int xPos, zPos;

private:
	bool loaded;

	std::vector<vertex> vertices;
	std::vector<unsigned int> indices;
	GLuint vao, vbo, elementbuffer;


public:
	Chunk();
	Chunk(int x, int z);
	~Chunk();

	void load();
	void render(GLuint mvp_loc, glm::mat4 mvp);

private:
	void generateTerrain();
};
