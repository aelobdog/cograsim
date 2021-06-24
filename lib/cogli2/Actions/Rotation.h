/*
 * Rotation.h
 *
 *  Created on: 31 gen 2018
 *      Author: lorenzo
 */

#ifndef ACTIONS_ROTATION_H_
#define ACTIONS_ROTATION_H_

#include "BaseAction.h"

#include <glm/glm.hpp>

class Rotation: public BaseAction {
protected:
	glm::vec3 _axis;
	float _angle;
public:
	Rotation(Camera &camera, glm::vec3 axis, float angle);
	virtual ~Rotation();

	virtual void do_action();
	virtual void undo_action();
};

#endif /* ACTIONS_ROTATION_H_ */
