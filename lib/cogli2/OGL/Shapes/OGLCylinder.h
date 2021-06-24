/*
 * OGLCylinder.h
 *
 *  Created on: 03/nov/2013
 *      Author: Flavio 
 */

#ifndef OGLCYLINDER_H_
#define OGLCYLINDER_H_

#include "Mesh.h"

#include <memory>
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>

class OGLCylinder {
protected:
	static int _resolution;

public:
	OGLCylinder() = delete;
	virtual ~OGLCylinder() = delete;

	static std::shared_ptr<Mesh> get_mesh(int res);
};

#endif /* OGLCYLINDER_H_ */
