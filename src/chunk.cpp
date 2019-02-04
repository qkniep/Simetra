#include "chunk.hpp"

#include "thinks/poisson_disk_sampling/poisson_disk_sampling.hpp"


Chunk::Chunk(int x, int z) : xPos(x), zPos(z), loaded(false) {
}


void Chunk::load(int seed) {
	if (loaded) return;

	namespace pds = thinks::poisson_disk_sampling;

	constexpr auto radius = 2.75f;
	const auto min = std::array<float, 2>{{(float) xPos * chunkSizeX,       (float) zPos * chunkSizeZ}};
	const auto max = std::array<float, 2>{{(float) (xPos + 1) * chunkSizeX, (float) (zPos + 1) * chunkSizeZ}};

	poissonPoints = pds::PoissonDiskSampling(radius, min, max, 30, seed + zPos);

	loaded = true;
}


std::vector<std::array<float, 2>>* Chunk::getPoissonPoints() {
	return &poissonPoints;
}
