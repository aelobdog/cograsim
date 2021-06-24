/*
 * PatchyParticle.cpp
 *
 *  Created on: 17/dic/2013
 *      Author: lorenzo
 */

#include "PatchyParticle.h"
#include "../Drawers/Drawer.h"

Patch::Patch(glm::vec3 n_pos, float n_width, glm::vec4 n_color) {
	pos = n_pos;
	width = n_width;
	cosmax = cos(width);
	color = n_color;
}

Patch::~Patch() {

}

PatchyParticle::PatchyParticle(glm::vec3 pos, float r, glm::vec4 color) : Sphere(pos, r, color) {

}

PatchyParticle::~PatchyParticle() {

}

void PatchyParticle::rotate(glm::mat3 R, glm::vec3 wrt) {
	Shape::rotate(R, wrt);
	for(std::vector<Patch>::iterator it = _patches.begin(); it != _patches.end(); it++) {
		it->pos = R*it->pos;
	}
}

void PatchyParticle::add_patch(glm::vec3 pos, float width, glm::vec4 color) {
	_patches.push_back(Patch(pos, width, color));
}

void PatchyParticle::accept_drawer(Drawer &d) {
	d.visit(*this);
}
