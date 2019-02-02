#ifndef WORLD_HPP
#define WORLD_HPP

#include <queue>
#include <vector>

#include "chunk.hpp"


class World {
	int seed;
	std::vector<Chunk*> allChunks;
	std::queue<Chunk*> loadingChunks;
	unsigned int deleteIndex;
	unsigned int currentRow;

public:
	World(int s);
	~World();

	void load();
	void render(GLuint mvp_loc, glm::mat4 mvp);

	void loadNextRow();

private:
	void clean();
};

#endif  // WORLD_HPP
