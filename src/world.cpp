#include "world.hpp"

#include <queue>


World::World(int s) : seed(s), allChunks(4 * 7), deletePtr(0), currentRow(3) {
	allChunks[0] = Chunk(0, 0);

	allChunks[1] = Chunk(-1, 1);
	allChunks[7] = Chunk( 0, 1);
	allChunks[2] = Chunk( 1, 1);

	allChunks[3]  = Chunk(-2, 2);
	allChunks[8]  = Chunk(-1, 2);
	allChunks[14] = Chunk( 0, 2);
	allChunks[9]  = Chunk( 1, 2);
	allChunks[4]  = Chunk( 2, 2);

	allChunks[5]  = Chunk(-3, 3);
	allChunks[10] = Chunk(-2, 3);
	allChunks[15] = Chunk(-1, 3);
	allChunks[21] = Chunk( 0, 3);
	allChunks[16] = Chunk( 1, 3);
	allChunks[11] = Chunk( 2, 3);
	allChunks[6]  = Chunk( 3, 3);
}


void World::load() {
	for (size_t i = 0; i < allChunks.size(); ++i) {
		if (allChunks[i].zPos >= 0)
			allChunks[i].load();
	}
}


void World::render(GLuint mvp_loc, glm::mat4 mvp) {
	for (size_t i = 0; i < allChunks.size(); ++i) {
		allChunks[i].render(mvp_loc, mvp);
	}
}


void World::loadNextRow() {
	for (int i = deletePtr; i < deletePtr + 7; ++i) {
		allChunks[i] = Chunk(-1, -1);
	}

	++currentRow;
	allChunks[(deletePtr - 3 * 7) % 7 + 5] = Chunk(-3, currentRow);
	allChunks[(deletePtr - 2 * 7) % 7 + 3] = Chunk(-2, currentRow);
	allChunks[(deletePtr - 1 * 7) % 7 + 1] = Chunk(-1, currentRow);
	allChunks[deletePtr]                   = Chunk( 0, currentRow);
	allChunks[(deletePtr - 1 * 7) % 7 + 2] = Chunk( 1, currentRow);
	allChunks[(deletePtr - 2 * 7) % 7 + 4] = Chunk( 2, currentRow);
	allChunks[(deletePtr - 3 * 7) % 7 + 6] = Chunk( 3, currentRow);
	deletePtr += 7;

	load();
}
