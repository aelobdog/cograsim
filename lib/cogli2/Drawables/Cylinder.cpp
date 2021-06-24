/*
 * Cylinder.cpp
 *
 *  Created on: 22/ott/2013
 *      Author: lorenzo
 */

#include "Cylinder.h"
#include "../Drawers/Drawer.h"

using glm::vec3;
using glm::vec4;

Cylinder::Cylinder(vec3 pos, glm::vec3 axis, float r, vec4 color) : Shape(pos), _axis(axis), _r(r), _color(color) {

}

Cylinder::~Cylinder() {

}

void Cylinder::accept_drawer(Drawer &d) {
	d.visit(*this);
}

void Cylinder::rotate(glm::mat3 R, glm::vec3 wrt) {
	Shape::rotate(R, wrt);
	_axis = R*_axis;
}
