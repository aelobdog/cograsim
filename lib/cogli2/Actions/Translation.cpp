/*
 * Translation.cpp
 *
 *  Created on: 31 gen 2018
 *      Author: lorenzo
 */

#include "Translation.h"

#include "../Camera.h"

Translation::Translation(Camera &camera, glm::vec3 direction, float extent) :
				BaseAction(camera),
				_direction(glm::normalize(direction)),
				_extent(extent) {

}

Translation::~Translation() {

}

void Translation::do_action() {
	_camera.translate(_direction, _extent);
}

void Translation::undo_action() {
	_camera.translate(_direction, -_extent);
}
