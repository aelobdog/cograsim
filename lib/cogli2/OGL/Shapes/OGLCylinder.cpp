/*
 * OGLCylinder.cpp
 *
 *  Created on: 03/nov/2013
 *      Author: Flavio
 */

#include "OGLCylinder.h"
#include "../../Utils/MathHelper.h"

int OGLCylinder::_resolution = -1;

std::shared_ptr<Mesh> OGLCylinder::get_mesh(int res) {
	// we assume
	// radius = 1.
	// height = (0, 0, 1)
	// the center is on the origin
	// so the solid is defined by (x^2 + y^2 < 1 && |z| < 1/2)

	std::vector<Vertex> vertices;
	vertices.reserve(2 * res * res + 2 * res);
	for(int r = 0; r < res; r++) {
		for(int s = 0; s < res; s++) {
			float const alpha = s * 2.f * glm::pi<float>() / (float) res;
			float const rho = r * 1.f / (float) (res - 1.f);
			float const x = rho * cosf(alpha);
			float const y = rho * sinf(alpha);

			Vertex v;
			v.position[0] = x;
			v.position[1] = y;
			v.position[2] = -0.5f;

			v.normal[0] = 0.f;
			v.normal[1] = 0.f;
			v.normal[2] = -1.f;

			vertices.emplace_back(v);
		}
	}

	std::vector<unsigned int> indices;
	indices.reserve(12 * res * (res - 1) + 6 * res);
	for(int r = 0; r < res - 1; r++) {
		for(int s = 0; s < res - 1; s++) {
			indices.push_back(r * res + s);
			indices.push_back(r * res + (s + 1));
			indices.push_back((r + 1) * res + (s + 1));

			indices.push_back((r + 1) * res + (s + 1));
			indices.push_back((r + 1) * res + s);
			indices.push_back(r * res + s);
		}
		indices.push_back(r * res + res - 1);
		indices.push_back(r * res);
		indices.push_back((r + 1) * res);

		indices.push_back((r + 1) * res);
		indices.push_back((r + 1) * res + res - 1);
		indices.push_back(r * res + res - 1);
	}
	unsigned int ind_base_idx = vertices.size();

	// tube
	for(int r = 0; r < 2; r++) {
		float const z = r - 0.5f;
		for(int s = 0; s < res; s++) {
			float const x = cosf(s * 2.f * glm::pi<float>() / res);
			float const y = sinf(s * 2.f * glm::pi<float>() / res);

			Vertex v;
			v.position[0] = x;
			v.position[1] = y;
			v.position[2] = z;

			v.normal[0] = x;
			v.normal[1] = y;
			v.normal[2] = 0.f;

			vertices.emplace_back(v);
		}
	}

	for(int s = 0; s < res - 1; s++) {
		indices.push_back(ind_base_idx + s);
		indices.push_back(ind_base_idx + (s + 1));
		indices.push_back(ind_base_idx + res + (s + 1));

		indices.push_back(ind_base_idx + res + (s + 1));
		indices.push_back(ind_base_idx + res + s);
		indices.push_back(ind_base_idx + s);
	}
	indices.push_back(ind_base_idx + res - 1);
	indices.push_back(ind_base_idx + 0);
	indices.push_back(ind_base_idx + res);

	indices.push_back(ind_base_idx + res);
	indices.push_back(ind_base_idx + res + res - 1);
	indices.push_back(ind_base_idx + res - 1);

	ind_base_idx = vertices.size();

	// upper face
	for(int r = 0; r < res; r++) {
		for(int s = 0; s < res; s++) {
			float const alpha = s * 2.f * glm::pi<float>() / (float) res;
			float const rho = r * 1.f / (float) (res - 1.f);

			float const x = rho * cosf(alpha);
			float const y = rho * sinf(alpha);

			Vertex v;
			v.position[0] = x;
			v.position[1] = y;
			v.position[2] = 0.5f;

			v.normal[0] = 0.f;
			v.normal[1] = 0.f;
			v.normal[2] = 1.f;

			vertices.emplace_back(v);
		}
	}

	for(int r = 0; r < res - 1; r++) {
		for(int s = 0; s < res - 1; s++) {
			indices.push_back(ind_base_idx + r * res + s);
			indices.push_back(ind_base_idx + r * res + (s + 1));
			indices.push_back(ind_base_idx + (r + 1) * res + (s + 1));

			indices.push_back(ind_base_idx + (r + 1) * res + (s + 1));
			indices.push_back(ind_base_idx + (r + 1) * res + s);
			indices.push_back(ind_base_idx + r * res + s);
		}
		indices.push_back(ind_base_idx + r * res + res - 1);
		indices.push_back(ind_base_idx + r * res);
		indices.push_back(ind_base_idx + (r + 1) * res);

		indices.push_back(ind_base_idx + (r + 1) * res);
		indices.push_back(ind_base_idx + (r + 1) * res + res - 1);
		indices.push_back(ind_base_idx + r * res + res - 1);
	}

	OGLCylinder::_resolution = res;

	return std::make_shared<Mesh>(vertices, indices);
}
