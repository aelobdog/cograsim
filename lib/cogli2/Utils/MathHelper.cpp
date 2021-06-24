/*
 * MathHelper.cpp
 *
 *  Created on: 06/nov/2013
 *      Author: lorenzo
 */

#include <cstdlib>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/random.hpp>
#include <glm/gtx/perpendicular.hpp>

#include "Logging.h"
#include "MathHelper.h"

namespace MathHelper {

using glm::vec3;
using glm::dvec3;

glm::mat4 get_rotation(glm::vec3 initial, glm::vec3 final) {
	initial = glm::normalize(initial);
	final = glm::normalize(final);

	float dot_res = glm::dot(initial, final);

	if(dot_res >= 0.99999f) {
		return glm::mat4();
	}
	else if(dot_res <= -0.99999f) {
		glm::mat4 res;
		res[0][0] = -1.;
		res[1][1] = -1.;
		res[2][2] = -1.;
		return res;
	}

	// Get CROSS product (the axis of rotation)
	vec3 t = glm::cross(initial, final);
	glm::normalize(t);
	float angle = glm::degrees(acosf(dot_res));
	if(dot_res > 1.f) {
		angle = 0.f;
	}
	if(dot_res < -1.f) {
		angle = (float) M_PI;
	}

	return glm::rotate(angle, t);
}

void orthonormalize(glm::dmat4 &m) {
	dvec3 v1(m[0][0], m[0][1], m[0][2]);
	dvec3 v2(m[1][0], m[1][1], m[1][2]);
	dvec3 v3(m[2][0], m[2][1], m[2][2]);

	v1 = glm::normalize(v1);
	v2 = v2 - glm::dot(v1, v2) / glm::length(v2);
	v2 = glm::normalize(v2);
	v3 = v3 - glm::dot(v3, v1) / glm::length(v3) - glm::dot(v3, v2) / glm::length(v3);
	v3 = glm::normalize(v3);

	m[0][0] = v1[0];
	m[0][1] = v1[1];
	m[0][2] = v1[2];

	m[1][0] = v2[0];
	m[1][1] = v2[1];
	m[1][2] = v2[2];

	m[2][0] = v3[0];
	m[2][1] = v3[1];
	m[2][2] = v3[2];
}

void eigenvalues_rotate(glm::mat3 &mat, double &c, double &s, int i0, int j0, int i1, int j1) {
	double a = c * mat[i0][j0] - s * mat[i1][j1];
	double b = s * mat[i0][j0] + c * mat[i1][j1];
	mat[i0][j0] = a;
	mat[i1][j1] = b;
}

void diagonalize_3x3(glm::mat3 &mat, glm::vec3 &values, glm::mat3 &vectors) {
	glm::mat3 tmp_vectors;
	values = eigenvalues_jacobi(mat, tmp_vectors);

	// sort solutions by eigenvalues
	for(int i = 0; i < 3; i++) {
		int k = i;
		double val = values[i];

		for(int j = i + 1; j < 3; j++)
			if(values[j] < val) {
				k = j;
				val = values[k];
			}

		if(k != i) {
			values[k] = values[i];
			values[i] = val;
			for(int j = 0; j < 3; j++) {
				val = tmp_vectors[j][i];
				tmp_vectors[j][i] = tmp_vectors[j][k];
				tmp_vectors[j][k] = val;
			}
		}
	}

	// transpose such that first index refers to solution index
	for(int i = 0; i < 3; i++) {
		for(int j = 0; j < 3; j++) vectors[i][j] = tmp_vectors[j][i];
	}
}

glm::vec3 eigenvalues_jacobi(glm::mat3 &mat, glm::mat3 &E) {
	glm::vec3 ret;

	// initial eigenvalues
	ret[0] = mat[0][0];
	ret[1] = mat[1][1];
	ret[2] = mat[2][2];

	E = glm::mat3();
	mat[0][0] = 1.0;
	mat[1][1] = 1.0;
	mat[2][2] = 1.0;

	for(int z = 0; z < 50; z++) {
		/* sum off-diagonal elements */
		double sum = 0.0;
		for(int i = 0; i < 2; i++) {
			for(int j = i + 1; j < 3; j++)
				sum += fabs(mat[i][j]);
		}

		/* if converged to machine underflow */
		if(sum == 0.0) return ret;

		for(int i = 0; i < 3; i++) {
			for(int j = i + 1; j < 3; j++) {
				double mii = ret[i];
				double mjj = ret[j];
				double mij = mat[i][j];
				double phi = 0.5 * atan2(2 * mij, mjj - mii);
				double c = cos(phi);
				double s = sin(phi);
				double mii1 = c * c * mii - 2 * s * c * mij + s * s * mjj;
				double mjj1 = s * s * mii + 2 * s * c * mij + c * c * mjj;
				if(abs(mii - mii1) < 0.00001 || (mjj - mjj1) < 0.00001) {
					ret[i] = mii1;
					ret[j] = mjj1;
					mat[i][j] = 0.0;
					for(int k = 0; k < i; k++) eigenvalues_rotate(mat, c, s, k, i, k, j);
					for(int k = i + 1; k < j; k++) eigenvalues_rotate(mat, c, s, i, k, k, j);
					for(int k = j + 1; k < 3; k++) eigenvalues_rotate(mat, c, s, i, k, j, k);
					for(int k = 0; k < 3; k++) eigenvalues_rotate(E, c, s, k, i, k, j);
				}
			}
		}
	}

	Logging::log_critical("Too many iterations in eigenvalues_jacobi\n");
	exit(1);

	return ret;
}

}

