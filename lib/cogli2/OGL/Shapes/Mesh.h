#ifndef MESH_H
#define MESH_H

#include <glad/glad.h> // holds all OpenGL type declarations

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <cstddef>

struct Vertex {
	// position
	glm::vec3 position;
	// normal
	glm::vec3 normal;
};

class Mesh {
public:
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices);
	~Mesh();
	Mesh(const Mesh &other) = delete;
	Mesh(Mesh &&other) = delete;

	bool has_instance_data();
	void add_instance_data(glm::mat4 model_matrix, glm::mat3 inverse_model_matrix, glm::vec4 color);
	void sort_instance_data(glm::vec3 camera_pos);
	void clear_instance_data();

	void draw();
	void draw_instances();

private:
	std::vector<Vertex> _vertices;
	std::vector<unsigned int> _indices;

	unsigned int VAO = 0;
	unsigned int VBO = 0;
	unsigned int EBO = 0;

	std::vector<glm::mat4> _instance_model_matrices;
	std::vector<glm::mat3> _instance_inverse_model_matrices;
	std::vector<glm::vec4> _instance_colors;

	unsigned int MMO = 0;
	unsigned int IMO = 0;
	unsigned int CO = 0;

	void _setup_mesh();
};
#endif
