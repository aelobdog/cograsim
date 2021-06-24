/*
 * Translation.h
 *
 *  Created on: 31 gen 2018
 *      Author: lorenzo
 */

#ifndef ACTIONS_TRANSLATION_H_
#define ACTIONS_TRANSLATION_H_

#include "BaseAction.h"

#include <glm/glm.hpp>

class Translation: public BaseAction {
protected:
	glm::vec3 _direction;
	float _extent;
public:
	Translation(Camera &camera, glm::vec3 direction, float extent);
	virtual ~Translation();

	virtual void do_action();
	virtual void undo_action();
};

#endif /* ACTIONS_TRANSLATION_H_ */
