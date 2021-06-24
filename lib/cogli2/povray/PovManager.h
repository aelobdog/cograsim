/*
 * PovManager.h
 *
 *  Created on: 07/nov/2013
 *      Author: lorenzo
 */

#ifndef POVMANAGER_H_
#define POVMANAGER_H_

#include <string>

#include "../Drawers/PovDrawer.h"
#include "../Drawables/Scene.h"
#include "../optionparser.h"
#include "../Camera.h"
#include "../Lighting.h"

class PovManager {
protected:
	option::Option *_options;
	PovDrawer _drawer;

public:
	PovManager(option::Option *options);
	virtual ~PovManager();

	void print_scene(Scene &scene, Camera &camera, LightingState &lighs);
};

#endif /* POVMANAGER_H_ */
