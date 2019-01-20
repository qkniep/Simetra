#include <queue>
#include <vector>

#include "chunk.hpp"


class World {
	int seed;
	std::vector<Chunk> allChunks;
	std::queue<Chunk*> loadingChunks;

public:
	World(int s);

	void load();
	void render(GLuint mvp_loc, glm::mat4 mvp);
};
