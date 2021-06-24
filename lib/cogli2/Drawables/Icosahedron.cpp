/*
 * Icosahedron.cpp
 *
 *  Created on: 16/luh/2018
 *      Author: flavio
 */

#include "Icosahedron.h"
#include "../Drawers/Drawer.h"

using glm::vec3;
using glm::vec4;

IcoPatch::IcoPatch(glm::vec3 n_pos, float n_radius, glm::vec4 n_color) {
	pos = n_pos;
	radius = n_radius;
	color = n_color;
}

IcoPatch::~IcoPatch() {

}

Icosahedron::Icosahedron(vec3 pos, glm::vec3 axis, glm::vec3 axis2, float r, vec4 color) : Shape(pos), _axis(axis), _axis2(axis2), _r(r), _color(color) {

}

Icosahedron::~Icosahedron() {

}

void Icosahedron::add_patch(glm::vec3 pos, float radius, glm::vec4 color) {
	_patches.push_back(IcoPatch(pos, radius, color));
}

void Icosahedron::accept_drawer(Drawer &d) {
	d.visit(*this);
}

void Icosahedron::rotate(glm::mat3 R, glm::vec3 wrt) {
	Shape::rotate(R, wrt);
	_axis = R*_axis;
	_axis2 = R*_axis2;
	for(std::vector<IcoPatch>::iterator it = _patches.begin(); it != _patches.end(); it++) {
		it->pos = R*it->pos;
	}
}
