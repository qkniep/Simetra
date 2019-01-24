#ifndef WORLD_HPP
#define WORLD_HPP

#include <queue>
#include <vector>

#include "chunk.hpp"


class World {
	int seed;
	std::vector<Chunk*> allChunks;
	std::queue<Chunk*> loadingChunks;
	int deletePtr;
	int currentRow;

public:
	World(int s);

	void load();
	void render(GLuint mvp_loc, glm::mat4 mvp);

	void loadNextRow();
	void clean();
};

#endif  // WORLD_HPP
