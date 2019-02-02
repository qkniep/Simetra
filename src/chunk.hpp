#ifndef CHUNK_HPP
#define CHUNK_HPP

#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "FastNoise/FastNoise.h"


struct vertex {
	GLfloat x, y, z;
	GLfloat r, g, b;
	GLfloat nx, ny, nz;
};


class Chunk {
public:
	static const int chunkSizeX = 192;
	static const int chunkSizeZ = 96;

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
	vertex generateVertex(const FastNoise& noise, float x, float z);
	void calculateNormals(unsigned int* v0);
	void generateDelaunayTerrain();
	void placeTrees();
};

#endif  // CHUNK_HPP
