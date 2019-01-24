#ifndef CHUNK_HPP
#define CHUNK_HPP

#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>


struct vertex {
	GLfloat x, y, z;
	GLfloat r, g, b;
};


class Chunk {
public:
	static const int chunkSize = 48;

private:
	int xPos, zPos;
	bool loaded;

	std::vector<vertex> vertices;
	std::vector<unsigned int> indices;
	GLuint vao, vbo, elementbuffer;


public:
	Chunk(int x, int z);
	~Chunk();

	void load();
	void render(GLuint mvp_loc, glm::mat4 mvp);

private:
	void generateTerrain();
};

#endif  // CHUNK_HPP
