/*
 * Spinning.cpp
 *
 *  Created on: 01 feb 2018
 *      Author: lorenzo
 */

#include "Spin.h"

#include "../Camera.h"

using namespace glm;

Spin::Spin(Camera &camera, glm::vec3 axis, float angle) :
				Rotation(camera, axis, angle) {

}

Spin::~Spin() {

}

void Spin::do_action() {
	_camera.rotate_look_at_vector(_axis, _angle);
}

void Spin::undo_action() {
	_camera.rotate_look_at_vector(_axis, -_angle);
}
