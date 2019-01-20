#include "chunk.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <stdlib.h>
#include <math.h>

#include "FastNoise/FastNoise.h"


Chunk::Chunk() : xPos(-1), zPos(-1) {
}


Chunk::Chunk(int x, int z) : xPos(x), zPos(z) {
}


void Chunk::load() {
	generateTerrain();

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(struct vertex), &vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(struct vertex), (void*) 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(struct vertex), (void*) (sizeof(float) * 3));

	glGenBuffers(1, &elementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
}


void Chunk::generateTerrain() {
	FastNoise noise = FastNoise(47);

	for (int x = 0; x < chunkSize+1; ++x) {
		for (int z = 0; z < chunkSize+1; ++z) {
			float fx = x, fz = z;
			int xx = xPos * chunkSize + x;
			int zz = zPos * chunkSize + z;
			//int biome = (int) round(1.5f * (noise.GetCellular(x, z) + 1));
			float height = noise.GetSimplex(xx, zz)
				+ .5f  * noise.GetSimplex(2 * xx, 2 * zz)
				+ .25f * noise.GetSimplex(4 * xx, 4 * zz);

			//if (biome < 3) height = -1.75;

			float colorR = 0, colorG = 0, colorB = 0;
			if (height < -0.5f) {
				float r = rand() % 24 / 256.f;
				colorR = 0.407f * (1 + r);
				colorG = 0.427f * (1 + r);
				colorB = 0.878f * (1 + r);
			} else if (height < 0.6f) {
				float r = rand() % 24 / 256.f;
				colorR = 0.4140625f * (1 + r);
				colorG = 0.6875f * (1 + r);
				colorB = 0.296875f * (1 + r);
			} else if (height < 0.9) {
				height += .5f * noise.GetSimplex(8 * xx, 8 * zz);
				int r = rand() % 32;
				colorR = (160 + r) / 256.f;
				colorG = (160 + r) / 256.f;
				colorB = (160 + r) / 256.f;
			} else {
				height += .5f * noise.GetSimplex(8 * xx, 8 * zz);
				colorR = (230 + rand() % 16) / 256.f;
				colorG = (230 + rand() % 16) / 256.f;
				colorB = 1;
			}

			height += 1.75;
			height *= 0.4f;
			height = powf(height, 4.6);
			height *= 10;

			vertices.push_back({fx, height, -fz, colorR, colorG, colorB});
		}
	}

	for (int x = 0; x < chunkSize; ++x) {
		for (int z = 0; z < chunkSize; ++z) {
			unsigned int v = x * (chunkSize + 1) + z;
			unsigned int quad[] = {
				v, v + chunkSize + 1, v + chunkSize + 2,
				v, v + chunkSize + 2, v + 1,
			};
			indices.insert(indices.end(), quad, quad + 6);
		}
	}
}


void Chunk::render(GLuint mvp_loc, glm::mat4 mvp) {
	glm::mat4 m(1);
	m = glm::translate(m, glm::vec3(xPos * chunkSize, 0, zPos * chunkSize));
	mvp = mvp * m;
	glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, glm::value_ptr(mvp));
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (void*) 0);
}
