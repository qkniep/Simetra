#ifndef CHUNK_HPP
#define CHUNK_HPP

#include <vector>


class Chunk {
public:
	static const int chunkSizeX = 96;
	static const int chunkSizeZ = 64;

private:
	int xPos, zPos;
	bool loaded;

	std::vector<std::array<float, 2>> poissonPoints;


public:
	Chunk(int x, int z);

	void load(int seed);
	std::vector<std::array<float, 2>>* getPoissonPoints();
};

#endif  // CHUNK_HPP
