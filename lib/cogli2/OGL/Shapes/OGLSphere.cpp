/*
 * OGLSphere.cpp
 *
 *  Created on: 03/nov/2013
 *      Author: lorenzo
 */

#include "OGLSphere.h"
#include "../utils.h"
#include "../../Utils/MathHelper.h"

int OGLSphere::_resolution = -1;

std::shared_ptr<Mesh> OGLSphere::get_mesh(int res) {
	float radius = 1.f;
	float const R = 1.f / (float) (res - 1);
	float const S = 1.f / (float) (res - 1);

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	vertices.resize(res * res);
	auto v = vertices.begin();
	for(int r = 0; r < res; r++) {
		for(int s = 0; s < res; s++) {
			float const y = sinf(-glm::half_pi<float>() + glm::pi<float>() * r * R);
			float const x = cosf(2.f * glm::pi<float>() * s * S) * sinf(glm::pi<float>() * r * R);
			float const z = sinf(2.f * glm::pi<float>() * s * S) * sinf(glm::pi<float>() * r * R);

			v->position[0] = x * radius;
			v->position[1] = y * radius;
			v->position[2] = z * radius;

			v->normal[0] = x;
			v->normal[1] = y;
			v->normal[2] = z;

			v++;
		}
	}

	indices.resize(res * res * 6);
	auto i = indices.begin();
	for(int r = 0; r < res - 1; r++) {
		for(int s = 0; s < res - 1; s++) {
			*i++ = r * res + s;
			*i++ = r * res + (s + 1);
			*i++ = (r + 1) * res + (s + 1);

			*i++ = (r + 1) * res + (s + 1);
			*i++ = (r + 1) * res + s;
			*i++ = r * res + s;
		}
	}

	return std::make_shared<Mesh>(vertices, indices);
}
