#include "world.hpp"


World::World(int s) : seed(s), allChunks(3, nullptr), deleteIndex(0), currentRow(3) {
	allChunks[0] = new Chunk(0, 0);
	allChunks[1] = new Chunk(0, 1);
	allChunks[2] = new Chunk(0, 2);
}

World::~World() {
	clean();
}


void World::load() {
	for (Chunk *c : allChunks) {
		if (c == nullptr) continue;
		c->load();
	}
}


void World::render(GLuint mvp_loc, glm::mat4 mvp) {
	for (Chunk *c : allChunks) {
		if (c == nullptr) continue;
		c->render(mvp_loc, mvp);
	}
}


void World::loadNextRow() {
	delete allChunks[deleteIndex];
	allChunks[deleteIndex] = new Chunk(0, currentRow);
	allChunks[deleteIndex]->load();
	deleteIndex = (deleteIndex + 1) % allChunks.size();
	currentRow++;
}


void World::clean() {
	for (Chunk* c : allChunks) {
		if (c == nullptr) continue;
		delete c;
	}
}
