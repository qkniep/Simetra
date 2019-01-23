#include "world.hpp"

#include <iostream>
#include <queue>


World::World(int s) : seed(s), allChunks(4 * 7, NULL), deletePtr(21), currentRow(3) {
	allChunks[0] = new Chunk(0, 0);

	allChunks[1] = new Chunk(-1, 1);
	allChunks[7] = new Chunk( 0, 1);
	allChunks[2] = new Chunk( 1, 1);

	allChunks[3]  = new Chunk(-2, 2);
	allChunks[8]  = new Chunk(-1, 2);
	allChunks[14] = new Chunk( 0, 2);
	allChunks[9]  = new Chunk( 1, 2);
	allChunks[4]  = new Chunk( 2, 2);

	/*allChunks[(deletePtr - 3 * 7) % allChunks.size() + 5] = new Chunk(-3, currentRow);
	allChunks[(deletePtr - 2 * 7) % allChunks.size() + 3] = new Chunk(-2, currentRow);
	allChunks[(deletePtr - 1 * 7) % allChunks.size() + 1] = new Chunk(-1, currentRow);
	allChunks[deletePtr]                                  = new Chunk( 0, currentRow);
	allChunks[(deletePtr - 1 * 7) % allChunks.size() + 2] = new Chunk( 1, currentRow);
	allChunks[(deletePtr - 2 * 7) % allChunks.size() + 4] = new Chunk( 2, currentRow);
	allChunks[(deletePtr - 3 * 7) % allChunks.size() + 6] = new Chunk( 3, currentRow);*/
	allChunks[5]  = new Chunk(-3, 3);
	allChunks[10] = new Chunk(-2, 3);
	allChunks[15] = new Chunk(-1, 3);
	allChunks[21] = new Chunk( 0, 3);
	allChunks[16] = new Chunk( 1, 3);
	allChunks[11] = new Chunk( 2, 3);
	allChunks[6]  = new Chunk( 3, 3);
	deletePtr = 0;
}


void World::load() {
	for (Chunk *c : allChunks) {
		if (c == NULL) continue;
		c->load();
	}
}


void World::render(GLuint mvp_loc, glm::mat4 mvp) {
	for (Chunk *c : allChunks) {
		if (c == NULL) continue;
		c->render(mvp_loc, mvp);
	}
}


void World::loadNextRow() {
	for (int i = deletePtr; i < deletePtr + 7; ++i) {
		if (allChunks[i] == NULL) continue;
		delete allChunks[i];
	}

	currentRow += 1;
	allChunks[(deletePtr + 1 * 7) % allChunks.size() + 5] = new Chunk(-3, currentRow);
	allChunks[(deletePtr + 2 * 7) % allChunks.size() + 3] = new Chunk(-2, currentRow);
	allChunks[(deletePtr + 3 * 7) % allChunks.size() + 1] = new Chunk(-1, currentRow);
	allChunks[deletePtr]                                  = new Chunk( 0, currentRow);
	allChunks[(deletePtr + 3 * 7) % allChunks.size() + 2] = new Chunk( 1, currentRow);
	allChunks[(deletePtr + 2 * 7) % allChunks.size() + 4] = new Chunk( 2, currentRow);
	allChunks[(deletePtr + 1 * 7) % allChunks.size() + 6] = new Chunk( 3, currentRow);
	std::cout << "i: " << (deletePtr + 1 * 7) % allChunks.size() + 5 << std::endl;
	std::cout << "i: " << (deletePtr + 2 * 7) % allChunks.size() + 3 << std::endl;
	std::cout << "i: " << (deletePtr + 3 * 7) % allChunks.size() + 1 << std::endl;
	std::cout << "i: " << deletePtr << std::endl;
	std::cout << "i: " << (deletePtr + 3 * 7) % allChunks.size() + 2 << std::endl;
	std::cout << "i: " << (deletePtr + 2 * 7) % allChunks.size() + 4 << std::endl;
	std::cout << "i: " << (deletePtr + 1 * 7) % allChunks.size() + 6 << std::endl;
	deletePtr += 7;
	deletePtr %= allChunks.size();

	load();
}


void World::clean() {
	for (Chunk* c : allChunks) {
		if (c == NULL) continue;
		delete c;
	}
}
