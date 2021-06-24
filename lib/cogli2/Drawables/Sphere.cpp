/*
 * Sphere.cpp
 *
 *  Created on: 30/ott/2013
 *      Author: lorenzo
 */

#include "Sphere.h"
#include "../Drawers/Drawer.h"

using glm::vec3;
using glm::vec4;

Sphere::Sphere(vec3 pos, float r, vec4 color) : Shape(pos), _color(color), _r(r) {

}

Sphere::~Sphere() {

}

void Sphere::accept_drawer(Drawer &d) {
	d.visit(*this);
}
