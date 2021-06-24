/*
 * DHS.cpp
 *
 *  Created on: 27/nov/2013
 *      Author: lorenzo
 */

#include "DHS.h"
#include "../Drawers/Drawer.h"

DHS::DHS(glm::vec3 pos, glm::vec3 dip, float r, glm::vec4 color) : Sphere(pos, r, color) {
	_dipole = dip;
	_cyl_r = 2 * r * DIP_THICKNESS;
	_cyl_length = glm::length(dip) * (1. - DIP_ARROW_RATIO);
}

DHS::~DHS() {

}

glm::vec3 DHS::dipole() const {
	return _dipole;
}

float DHS::dipole_cylinder_radius() const {
	return _cyl_r;
}

float DHS::dipole_cylinder_length() const {
	return _cyl_length;
}

void DHS::accept_drawer(Drawer &d) {
	d.visit(*this);
}

void DHS::rotate(glm::mat3 R, glm::vec3 wrt) {
	Shape::rotate(R, wrt);
	_dipole = R*_dipole;
}
