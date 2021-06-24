/*
 * Spin.h
 *
 *  Created on: 01 feb 2018
 *      Author: lorenzo
 */

#ifndef ACTIONS_SPIN_H_
#define ACTIONS_SPIN_H_

#include "Rotation.h"

class Spin: public Rotation {
public:
	Spin(Camera &camera, glm::vec3 axis, float angle);
	virtual ~Spin();

	virtual void do_action();
	virtual void undo_action();
};

#endif /* ACTIONS_SPIN_H_ */
