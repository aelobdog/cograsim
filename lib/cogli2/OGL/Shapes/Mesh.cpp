/*
 * Mesh.cpp
 *
 *  Created on: Apr 13, 2020
 *      Author: lorenzo
 */

#include "Mesh.h"

#include "../../Utils/Generic.h"

#include <glm/gtx/norm.hpp>
#include <algorithm>

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices) {
	_vertices = vertices;
	_indices = indices;

	// now that we have all the required data, set the vertex buffers and its attribute pointers.
	_setup_mesh();
}

Mesh::~Mesh() {

}

bool Mesh::has_instance_data() {
	return (_instance_model_matrices.size() > 0);
}

void Mesh::add_instance_data(glm::mat4 model_matrix, glm::mat3 inverse_model_matrix, glm::vec4 color) {
	_instance_model_matrices.emplace_back(model_matrix);
	_instance_inverse_model_matrices.emplace_back(inverse_model_matrix);
	_instance_colors.emplace_back(color);
}

void Mesh::sort_instance_data(glm::vec3 camera_pos) {
	auto sort_lambda = [&camera_pos](const glm::mat4 &m1, const glm::mat4 &m2) {
		glm::vec3 pos1(m1[3]);
		glm::vec3 pos2(m2[3]);
		return glm::length2(camera_pos - pos1) > glm::length2(camera_pos - pos2);
	};

	auto perm = Generic::sort_permutation(_instance_model_matrices, sort_lambda);

	// the following lines result in an error, for some reason
	_instance_model_matrices = Generic::apply_permutation(_instance_model_matrices, perm);
	_instance_inverse_model_matrices = Generic::apply_permutation(_instance_inverse_model_matrices, perm);
	_instance_colors = Generic::apply_permutation(_instance_colors, perm);
}

void Mesh::clear_instance_data() {
	_instance_model_matrices.clear();
	_instance_inverse_model_matrices.clear();
	_instance_colors.clear();
}

void Mesh::draw() {
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Mesh::draw_instances() {
	int n_instances = _instance_model_matrices.size();

	glBindBuffer(GL_ARRAY_BUFFER, MMO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * n_instances, &_instance_model_matrices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, IMO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat3) * n_instances, &_instance_inverse_model_matrices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, CO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * n_instances, &_instance_colors[0], GL_STATIC_DRAW);

	glBindVertexArray(VAO);
	glDrawElementsInstanced(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0, n_instances);
	glBindVertexArray(0);
}

void Mesh::_setup_mesh() {
	// create buffers/arrays
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	// load data into vertex buffers
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// A great thing about structs is that their memory layout is sequential for all its items.
	// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
	// again translates to 3/2 floats which translates to a byte array.
	glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(Vertex), &_vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(unsigned int), &_indices[0], GL_STATIC_DRAW);

	// set the vertex attribute pointers
	// vertex Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) 0);
	// vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, normal));

	glGenBuffers(1, &MMO);
	glBindBuffer(GL_ARRAY_BUFFER, MMO);
	// model matrices are 4x4 matrices
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*) 0);
	glVertexAttribDivisor(2, 1);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*) (1 * sizeof(glm::vec4)));
	glVertexAttribDivisor(3, 1);
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*) (2 * sizeof(glm::vec4)));
	glVertexAttribDivisor(4, 1);
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*) (3 * sizeof(glm::vec4)));
	glVertexAttribDivisor(5, 1);

	glGenBuffers(1, &IMO);
	glBindBuffer(GL_ARRAY_BUFFER, IMO);
	// inverse model matrices are 3x3 matrices
	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, sizeof(glm::mat3), (void*) 0);
	glVertexAttribDivisor(6, 1);
	glEnableVertexAttribArray(7);
	glVertexAttribPointer(7, 3, GL_FLOAT, GL_FALSE, sizeof(glm::mat3), (void*) (1 * sizeof(glm::vec3)));
	glVertexAttribDivisor(7, 1);
	glEnableVertexAttribArray(8);
	glVertexAttribPointer(8, 3, GL_FLOAT, GL_FALSE, sizeof(glm::mat3), (void*) (2 * sizeof(glm::vec3)));
	glVertexAttribDivisor(8, 1);

	glGenBuffers(1, &CO);
	glBindBuffer(GL_ARRAY_BUFFER, CO);
	// colors are vec4
	glEnableVertexAttribArray(9);
	glVertexAttribPointer(9, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*) 0);
	glVertexAttribDivisor(9, 1);

	glBindVertexArray(0);
}
