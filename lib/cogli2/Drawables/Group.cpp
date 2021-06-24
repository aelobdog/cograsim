/*
 * Group.cpp
 *
 *  Created on: 13 May 2016
 *      Author: rovigattil
 */

#include "Group.h"

#include "../Drawers/Drawer.h"

using namespace std;

Group::Group() {

}

Group::~Group() {
	for(vector<Shape *>::iterator it = _shapes.begin(); it != _shapes.end(); it++) {
		delete *it;
	}
}

void Group::add_shape(Shape *s) {
	_shapes.push_back(s);
	_com += s->position();
}

void Group::accept_drawer(Drawer &d) {
	for(vector<Shape *>::iterator it = _shapes.begin(); it != _shapes.end(); it++) {
		Shape &s = *(*it);
		s.accept_drawer(d);
	}
}

void Group::shift(glm::vec3 &amount) {
	for(vector<Shape *>::iterator it = _shapes.begin(); it != _shapes.end(); it++) (*it)->shift(amount);

	_com += ((float)_shapes.size())*amount;
}

void Group::set_opacity(float n_op) {
	for(vector<Shape *>::iterator it = _shapes.begin(); it != _shapes.end(); it++) (*it)->set_opacity(n_op);
}
void Group::rotate(glm::mat3 R, glm::vec3 wrt) {
	for(vector<Shape *>::iterator it = _shapes.begin(); it != _shapes.end(); it++) {
		(*it)->rotate(R, wrt);
	}
}

glm::mat3 Group::inertia_tensor(glm::vec3 wrt) {
	glm::mat3 inertia(0.f);
	for(vector<Shape *>::iterator it = _shapes.begin(); it != _shapes.end(); it++) {
		inertia += (*it)->inertia_tensor(wrt);
	}
	return inertia;
}
