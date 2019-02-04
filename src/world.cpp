#include "world.hpp"

#include <math.h>
#include <iostream>

#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/normal.hpp>

#include "delabella/delabella.h"
#include "thinks/poisson_disk_sampling/poisson_disk_sampling.hpp"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tinyobjloader/tiny_obj_loader.h"


World::World(int s) : seed(s), allChunks(4 * 7, nullptr), deleteIndex(21), currentRow(3) {
	allChunks[0] = new Chunk(0, 0);

	allChunks[1] = new Chunk(-1, 1);
	allChunks[7] = new Chunk( 0, 1);
	allChunks[2] = new Chunk( 1, 1);

	allChunks[3]  = new Chunk(-2, 2);
	allChunks[8]  = new Chunk(-1, 2);
	allChunks[14] = new Chunk( 0, 2);
	allChunks[9]  = new Chunk( 1, 2);
	allChunks[4]  = new Chunk( 2, 2);

	allChunks[(deleteIndex + 1 * 7) % allChunks.size() + 5] = new Chunk(-3, currentRow);
	allChunks[(deleteIndex + 2 * 7) % allChunks.size() + 3] = new Chunk(-2, currentRow);
	allChunks[(deleteIndex + 3 * 7) % allChunks.size() + 1] = new Chunk(-1, currentRow);
	allChunks[deleteIndex]                                  = new Chunk( 0, currentRow);
	allChunks[(deleteIndex + 3 * 7) % allChunks.size() + 2] = new Chunk( 1, currentRow);
	allChunks[(deleteIndex + 2 * 7) % allChunks.size() + 4] = new Chunk( 2, currentRow);
	allChunks[(deleteIndex + 1 * 7) % allChunks.size() + 6] = new Chunk( 3, currentRow);
	deleteIndex = 0;
}


World::~World() {
	GLuint buffers[] = {vbo, elementbuffer};
	glDeleteBuffers(2, buffers);
	glDeleteVertexArrays(1, &vao);
}


void World::load() {
	samples.clear();

	GLuint buffers[] = {vbo, elementbuffer};
	glDeleteBuffers(2, buffers);
	glDeleteVertexArrays(1, &vao);

	for (Chunk *c : allChunks) {
		if (c == nullptr) continue;
		c->load(seed);
		auto pp = c->getPoissonPoints();
		samples.insert(samples.end(), pp->begin(), pp->end());
	}

	generateDelaunayTerrain(seed);
	//placeTrees(seed);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(struct vertex), &vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(struct vertex), (void*) 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(struct vertex), (void*) (sizeof(GLfloat) * 3));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(struct vertex), (void*) (sizeof(GLfloat) * 6));

	glGenBuffers(1, &elementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
}


void World::render(GLuint mvp_loc, glm::mat4 mvp) {
	glBindVertexArray(vao);

	glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, glm::value_ptr(mvp));
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (void*) 0);
}


void World::loadNextRow() {
	for (unsigned int i = deleteIndex; i < deleteIndex + 7; ++i) {
		if (allChunks[i] == nullptr) continue;
		delete allChunks[i];
		allChunks[i] = nullptr;
	}

	currentRow += 1;
	allChunks[(deleteIndex + 1 * 7) % allChunks.size() + 5] = new Chunk(-3, currentRow);
	allChunks[(deleteIndex + 2 * 7) % allChunks.size() + 3] = new Chunk(-2, currentRow);
	allChunks[(deleteIndex + 3 * 7) % allChunks.size() + 1] = new Chunk(-1, currentRow);
	allChunks[deleteIndex]                                  = new Chunk( 0, currentRow);
	allChunks[(deleteIndex + 3 * 7) % allChunks.size() + 2] = new Chunk( 1, currentRow);
	allChunks[(deleteIndex + 2 * 7) % allChunks.size() + 4] = new Chunk( 2, currentRow);
	allChunks[(deleteIndex + 1 * 7) % allChunks.size() + 6] = new Chunk( 3, currentRow);
	deleteIndex += 7;
	deleteIndex %= allChunks.size();

	load();
}


vertex World::generateVertex(const FastNoise& noise, float x, float z) {
	float height = noise.GetSimplex(1 * x, 1 * z)
		+ .75f  * noise.GetSimplex(2 * x, 2 * z)
		+ .5f * noise.GetSimplex(4 * x, 4 * z);
		//+ .25f * noise.GetSimplex(32 * globalX, 32 * globalZ);

	float colorR = 0, colorG = 0, colorB = 0;
	if (height < -0.9f) {
		colorR = 0.407f;
		colorG = 0.427f;
		colorB = 0.878f;
	} else if (height < 0.4f) {
		colorR = 0.4140625f;
		colorG = 0.6875f;
		colorB = 0.296875f;
		//colorR = 0.239f;
		//colorG = 0.369f;
		//colorB = 0.098f;
	} else if (height < 1.1f) {
		//height += (0.5f * noise.GetSimplex(8 * globalX, 8 * globalZ)) + 0.25f;
		colorR = 160 / 256.f;
		colorG = 160 / 256.f;
		colorB = 160 / 256.f;
	} else {
		//height += (0.5f * noise.GetSimplex(8 * globalX, 8 * globalZ)) + 0.25f;
		colorR = 230 / 256.f;
		colorG = 230 / 256.f;
		colorB = 1;
	}

	//height += 1.75;
	height += 2.25f;
	height *= 0.4f;  // do not change
	height = powf(height, 4.6);
	//height = powf(height, 6);
	//height *= 2.5f;
	height *= 5.f;

	return {x, height, z, colorR, colorG, colorB, 0, 0, 0};
}


void World::calculateNormals(unsigned int* i0) {
	glm::vec3 normal = triangleNormal(
			glm::vec3(vertices[*i0].x,     vertices[*i0].y,     vertices[*i0].z),
			glm::vec3(vertices[*(i0+1)].x, vertices[*(i0+1)].y, vertices[*(i0+1)].z),
			glm::vec3(vertices[*(i0+2)].x, vertices[*(i0+2)].y, vertices[*(i0+2)].z)
	);
	vertices[*(i0+2)].nx = normal.x;
	vertices[*(i0+2)].ny = normal.y;
	vertices[*(i0+2)].nz = normal.z;
}


void World::generateDelaunayTerrain(int seed) {
	vertices = std::vector<vertex>(samples.size());
	FastNoise noise = FastNoise(seed);
	for (size_t i = 0; i < samples.size(); ++i) {
		vertices[i] = generateVertex(noise, samples[i][0], samples[i][1]);
	}

	IDelaBella* idb = IDelaBella::Create();
	const int numIndices = idb->Triangulate(vertices.size(), &vertices.data()->x, &vertices.data()->z, sizeof(struct vertex));

	indices = std::vector<unsigned int>(numIndices);

	unsigned int i = 0;
	for (auto t = idb->GetFirstDelaunayTriangle(); t != nullptr; t = t->next) {
		indices[i]   = t->v[2]->i;
		indices[i+1] = t->v[1]->i;
		indices[i+2] = t->v[0]->i;
		auto *v = &vertices[indices[i+2]];
		if (v->nx != 0 || v->ny != 0 || v->nz != 0) {
			indices[i+2] = vertices.size();
			vertices.push_back(*v);
		}
		calculateNormals(&indices[i]);
		i += 3;
	}

	idb->Destroy();
}


void World::placeTrees(int /*seed*/) {
	/*tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string warn;
	std::string err;

	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, "models/tree_blocks.obj", "models/", true);

	if (!warn.empty()) {
		std::cout << "WARNING: " << warn << std::endl;
	}

	if (!err.empty()) {
		std::cerr << "ERROR: " << err << std::endl;
	}

	if (!ret) {
		std::cout << "ERROR: Failed to load .obj model." << std::endl;
	}

	namespace pds = thinks::poisson_disk_sampling;

	constexpr auto radius = 7.5f;
	const auto min = std::array<float, 2>{{0, 0}};
	const auto max = std::array<float, 2>{{chunkSizeX, chunkSizeZ}};

	const auto samples = pds::PoissonDiskSampling(radius, min, max, 30, rand());

	for (auto x : samples) {
		for (auto s : shapes) {
			std::cout << materials[s.mesh.material_ids[0]].diffuse[0] << std::endl;
			for (size_t i = 0; i < s.mesh.indices.size(); ++i) {
				indices.push_back(s.mesh.indices[i].vertex_index + vertices.size());
			}
		}

		for (size_t i = 0; i < attrib.vertices.size() / 3; ++i) {
			auto *p = &attrib.vertices[3*i];
			vertices.push_back({*p + x[0], *(p+1), *(p+2) + x[1], 0.352941f, 0.768628f, 0.713726f, 0, 0, 0});
		}
	}*/
}
