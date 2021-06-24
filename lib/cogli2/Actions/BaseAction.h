/*
 * BaseAction.h
 *
 *  Created on: 31 gen 2018
 *      Author: lorenzo
 */

#ifndef ACTIONS_BASEACTION_H_
#define ACTIONS_BASEACTION_H_

class Camera;

class BaseAction {
protected:
	Camera &_camera;
public:
	BaseAction(Camera &camera);
	virtual ~BaseAction();

	virtual void do_action() = 0;
	virtual void undo_action() = 0;
};

#endif /* ACTIONS_BASEACTION_H_ */
