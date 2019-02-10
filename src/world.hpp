#ifndef WORLD_HPP
#define WORLD_HPP

#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "chunk.hpp"
#include "FastNoise/FastNoise.h"


struct vertex {
	GLfloat x, y, z;
	GLfloat r, g, b;
	GLfloat nx, ny, nz;
};


class World {
	int seed;
	std::vector<Chunk*> allChunks;
	unsigned int deleteIndex;
	unsigned int currentRow;

	std::vector<std::array<float, 2>> samples;
	std::vector<vertex> vertices;
	std::vector<unsigned int> indices;
	GLuint vao, vbo, elementbuffer;


public:
	World(int s);
	~World();

	void load();
	void render(GLuint mvp_loc, glm::mat4 mvp);
	void setSeed(int s);

	void loadNextRow();

private:
	vertex generateVertex(const FastNoise& noise, float x, float z);
	void calculateNormals(unsigned int* v0);
	void generateDelaunayTerrain(int seed);
	void placeTrees(int seed);
	//void addWater();
};

#endif  // WORLD_HPP
