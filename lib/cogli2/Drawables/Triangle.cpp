/*
 * Triangle.cpp
 *
 *  Created on: 30/ott/2013
 *      Author: lorenzo
 */

#include "Triangle.h"
#include "../Drawers/Drawer.h"

using glm::vec3;
using glm::vec4;

Triangle::Triangle(vec3 v1, vec3 v2, vec3 v3, vec4 color) : Shape(v1), _color(color) {
	_vertices.push_back(v1);
	_vertices.push_back(v2);
	_vertices.push_back(v3);

	_position = (v1 + v2 + v3)/3.f;
}

Triangle::~Triangle() {

}

void Triangle::accept_drawer(Drawer &d) {
	d.visit(*this);
}

void Triangle::shift(vec3 &amount) {
	Shape::shift(amount);

	_vertices[0] += amount;
	_vertices[1] += amount;
	_vertices[2] += amount;
}
