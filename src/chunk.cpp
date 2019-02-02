#include "chunk.hpp"

#include <math.h>
#include <iostream>

#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/normal.hpp>

#include "thinks/poisson_disk_sampling/poisson_disk_sampling.hpp"
#include "delabella/delabella.h"


Chunk::Chunk(int x, int z) : xPos(x), zPos(z), loaded(false) {
}


Chunk::~Chunk() {
	if (!loaded) return;

	GLuint buffers[] = {vbo, elementbuffer};
	glDeleteBuffers(2, buffers);
	glDeleteVertexArrays(1, &vao);
}


void Chunk::load() {
	if (loaded) return;

	generateDelaunayTerrain();

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

	loaded = true;
}


void Chunk::render(GLuint mvp_loc, glm::mat4 mvp) {
	if (!loaded) return;

	glBindVertexArray(vao);

	glm::mat4 m(1);
	m = glm::translate(m, glm::vec3(xPos * chunkSize, 0, zPos * chunkSize));
	mvp = mvp * m;
	glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, glm::value_ptr(mvp));
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (void*) 0);
}


vertex Chunk::generateVertex(const FastNoise& noise, float x, float z) {
	float globalX = xPos * chunkSize + x;
	float globalZ = zPos * chunkSize + z;

	float height = noise.GetSimplex(4 * globalX, 4 * globalZ)
		+ .75f  * noise.GetSimplex(8 * globalX, 8 * globalZ)
		+ .5f * noise.GetSimplex(16 * globalX, 16 * globalZ);
		//+ .25f * noise.GetSimplex(32 * globalX, 32 * globalZ);

	float colorR = 0, colorG = 0, colorB = 0;
	if (height < -0.9f) {
		float r = rand() % 24 / 256.f;
		colorR = 0.407f * (1 + r);
		colorG = 0.427f * (1 + r);
		colorB = 0.878f * (1 + r);
	} else if (height < 0.5f) {
		//float r = rand() % 24 / 256.f;
		//colorR = 0.4140625f * (1 + r);
		//colorG = 0.6875f * (1 + r);
		//colorB = 0.296875f * (1 + r);
		colorR = 0.4140625f;
		colorG = 0.6875f;
		colorB = 0.296875f;
	} else if (height < 1.3f) {
		//height += (0.5f * noise.GetSimplex(8 * globalX, 8 * globalZ)) + 0.25f;
		int r = rand() % 32;
		colorR = (160 + r) / 256.f;
		colorG = (160 + r) / 256.f;
		colorB = (160 + r) / 256.f;
	} else {
		//height += (0.5f * noise.GetSimplex(8 * globalX, 8 * globalZ)) + 0.25f;
		colorR = (230 + rand() % 16) / 256.f;
		colorG = (230 + rand() % 16) / 256.f;
		colorB = 1;
	}

	//height += 1.75;
	height += 2.25f;
	height *= 0.4;  // do not change
	height = powf(height, 4.6);
	//height = powf(height, 6);
	height *= 2.5f;

	return {x, height, z, colorR, colorG, colorB, 0, 0, 0};
}


void Chunk::calculateNormals(unsigned int* i0) {
	glm::vec3 normal = triangleNormal(
			glm::vec3(vertices[*i0].x,     vertices[*i0].y,     vertices[*i0].z),
			glm::vec3(vertices[*(i0+1)].x, vertices[*(i0+1)].y, vertices[*(i0+1)].z),
			glm::vec3(vertices[*(i0+2)].x, vertices[*(i0+2)].y, vertices[*(i0+2)].z)
	);
	vertices[*(i0+2)].nx = normal.x;
	vertices[*(i0+2)].ny = normal.y;
	vertices[*(i0+2)].nz = normal.z;
}


void Chunk::generateDelaunayTerrain() {
	namespace pds = thinks::poisson_disk_sampling;

	constexpr auto radius = 2.75f;
	const auto min = std::array<float, 2>{{0, 0}};
	const auto max = std::array<float, 2>{{chunkSize, chunkSize}};

	const auto samples = pds::PoissonDiskSampling(radius, min, max, 30, rand());

	vertices = std::vector<vertex>(samples.size());
	FastNoise noise = FastNoise(47);
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
}
