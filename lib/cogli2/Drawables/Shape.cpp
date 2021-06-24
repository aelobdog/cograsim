/*
 * Shape.cpp
 *
 *  Created on: 03/nov/2013
 *      Author: lorenzo
 */

#include "Shape.h"

#include "../Utils/Logging.h"

Shape::Shape(glm::vec3 pos) : Drawable(), _is_pickable(false), _is_selected(false), _is_visible(true), _position(pos) {

}

Shape::~Shape() {

}

void Shape::_set_color_opacity(glm::vec4 &color, float n_op) {
	color[3] *= n_op;
	_is_visible = (n_op > 0.);
}

glm::mat3 Shape::inertia_tensor(glm::vec3 wrt) {
	glm::vec3 rel_pos = position() - wrt;
	// https://en.wikipedia.org/wiki/Moment_of_inertia#The_inertia_tensor
	return glm::mat3(1.f)*(glm::dot(rel_pos, rel_pos)) - glm::outerProduct(rel_pos, rel_pos);
}

void Shape::rotate(glm::mat3 R, glm::vec3 wrt) {
	_position = R*(_position - wrt) + wrt;
}
