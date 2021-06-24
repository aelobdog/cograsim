/*
 * Arrow.cpp
 *
 *  Created on: 22/ott/2013
 *      Author: lorenzo
 */

#include "Arrow.h"
#include "../Drawers/Drawer.h"
#include "../Utils/StringHelper.h"

using glm::vec3;
using glm::vec4;

Arrow::Arrow(glm::vec3 pos, glm::vec3 axis, float r_cyl, float cyl_length, glm::vec4 color) :
		Shape(pos), _axis(axis), _r_cyl(r_cyl), _cyl_length(cyl_length), _color(color) {
	_tot_length = glm::length(axis);
	_cone_length = _tot_length - cyl_length;
	if(_cone_length < 0.) {
		throw StringHelper::sformat("The length of the cones that are part of arrows must be > 0 (total length: %f, cylinder length: %f, cone length: %f\n", _tot_length, _cyl_length, _cone_length);
	}
}

Arrow::~Arrow() {

}

void Arrow::accept_drawer(Drawer &d) {
	d.visit(*this);
}

void Arrow::rotate(glm::mat3 R, glm::vec3 wrt) {
	Shape::rotate(R, wrt);
	_axis = R*_axis;
}
