#include "world.hpp"

#include <queue>


World::World(int s) : seed(s), allChunks(4) {
	allChunks[0] = Chunk(0, 0);
	allChunks[1] = Chunk(-1, -1);
	allChunks[2] = Chunk( 0, -1);
	allChunks[3] = Chunk( 1, -1);
}


void World::load() {
	for (size_t i = 0; i < allChunks.size(); ++i) {
		allChunks[i].load();
	}
}


void World::render(GLuint mvp_loc, glm::mat4 mvp) {
	for (size_t i = 0; i < allChunks.size(); ++i) {
		allChunks[i].render(mvp_loc, mvp);
	}
}
