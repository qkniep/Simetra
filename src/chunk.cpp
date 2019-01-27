#include "chunk.hpp"

#include <math.h>

#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/normal.hpp>

#include "FastNoise/FastNoise.h"



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

	generateTerrain();

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


void Chunk::generateTerrain() {
	vertices = std::vector<vertex>(chunkSize * chunkSize * 2 + chunkSize * 2 + 1);
	FastNoise noise = FastNoise(47);

	int i = 0;
	for (int x = 0; x < chunkSize+1; ++x) {
		for (int z = 0; z < chunkSize+1; ++z) {
			float fx = x, fz = z;
			int globalX = xPos * chunkSize + x;
			int globalZ = zPos * chunkSize + z;
			//int biome = (int) round(1.5f * (noise.GetCellular(x, z) + 1));
			float height = noise.GetSimplex(globalX, globalZ)
				+ .5f  * noise.GetSimplex(2 * globalX, 2 * globalZ)
				+ .25f * noise.GetSimplex(4 * globalX, 4 * globalZ);

			//if (biome < 3) height = -1.75;

			float colorR = 0, colorG = 0, colorB = 0;
			if (height < -0.5f) {
				float r = rand() % 24 / 256.f;
				colorR = 0.407f * (1 + r);
				colorG = 0.427f * (1 + r);
				colorB = 0.878f * (1 + r);
			} else if (height < 0.9f) {
				float r = rand() % 24 / 256.f;
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

			height += 1.75;
			height *= 0.4f;
			height = powf(height, 4.6);
			height *= 10;

			vertices[i] = {fx, height, fz, colorR, colorG, colorB, 0, 0, 0};
			if (x < chunkSize && z < chunkSize) {
				vertices[i + chunkSize + 1] = {fx, height, fz, colorR, colorG, colorB, 0, 0, 0};
			}
			i += 1;
		}
		i += chunkSize;
	}

	for (int x = 0; x < chunkSize; ++x) {
		for (int z = 0; z < chunkSize; ++z) {
			unsigned int v = x * (2 * (chunkSize + 1) - 1) + (chunkSize + 1) + z;
			unsigned int quad[] = {
				v + chunkSize + 1, v - chunkSize, v,
				v,                 v + chunkSize, v + chunkSize + 1
			};
			// compute normals
			for (int i = 0; i < 2; ++i) {
				glm::vec3 normal = triangleNormal(
						glm::vec3(vertices[quad[3*i]].x, vertices[quad[3*i]].y, vertices[quad[3*i]].z),
						glm::vec3(vertices[quad[3*i+1]].x, vertices[quad[3*i+1]].y, vertices[quad[3*i+1]].z),
						glm::vec3(vertices[quad[3*i+2]].x, vertices[quad[3*i+2]].y, vertices[quad[3*i+2]].z)
				);
				vertices[quad[3*i+2]].nx = normal.x;
				vertices[quad[3*i+2]].ny = normal.y;
				vertices[quad[3*i+2]].nz = normal.z;
			}

			indices.insert(indices.end(), quad, quad + 6);
		}
	}
}
