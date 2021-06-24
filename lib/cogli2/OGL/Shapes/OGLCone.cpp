/*
 * OGLCone.cpp
 *
 *  Created on: 12/nov/2013
 *      Author: lorenzo
 */

#include "OGLCone.h"
#include "../../Utils/MathHelper.h"

using std::vector;

int OGLCone::_resolution = -1;

std::shared_ptr<Mesh> OGLCone::get_mesh(int res, bool with_base) {
	// we assume
	// base radius = 1.
	// height = (0, 0, 1)
	// the midpoint is on the origin and the apex is at (0, 0, 0.5)

	std::vector<Vertex> vertices(2 * (res + 1));
	std::vector<unsigned int> indices(6 * res);

	// base
	auto v = vertices.begin();
	auto i = indices.begin();

	// the centre of the circle
	v->position[0] = 0;
	v->position[1] = 0;
	v->position[2] = -0.5f;

	v->normal[0] = 0;
	v->normal[1] = 0;
	v->normal[2] = -1.0f;

	v++;

	// the perimeter of the circle
	for(int s = 1; s <= res; s++) {
		float const x = cosf(s * 2.f * glm::pi<float>() / res);
		float const y = sinf(s * 2.f * glm::pi<float>() / res);

		v->position[0] = x;
		v->position[1] = y;
		v->position[2] = -0.5f;

		v->normal[0] = 0;
		v->normal[1] = 0;
		v->normal[2] = -1.0f;

		v++;

		*i++ = 0;
		*i++ = s;
		*i++ = (s == res) ? 1 : s + 1;
	}

	// the apex
	v->position[0] = 0.f;
	v->position[1] = 0.f;
	v->position[2] = 0.5f;

	v->normal[0] = 0.f;
	v->normal[1] = 0.f;
	v->normal[2] = 1.f;

	v++;

	int base_idx = res + 1;
	for(int s = 1; s <= res; s++) {
		float x = cosf(s * 2.f * glm::pi<float>() / res);
		float y = sinf(s * 2.f * glm::pi<float>() / res);

		v->position[0] = x;
		v->position[1] = y;
		v->position[2] = -0.5f;

		float norm_length = sqrtf(x*x + y*y + 1.0f);
		v->normal[0] = x / norm_length;
		v->normal[1] = y / norm_length;
		v->normal[2] = 1.0f;

		v++;

		*i++ = base_idx;
		*i++ = base_idx + s;
		*i++ = (s == res) ? base_idx + 1 : base_idx + s + 1;
	}

	return std::make_shared<Mesh>(vertices, indices);
}
