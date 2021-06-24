/*
 * OGLSphere.h
 *
 *  Created on: 03/nov/2013
 *      Author: lorenzo
 */

#ifndef OGLSPHERE_H_
#define OGLSPHERE_H_

#include "Mesh.h"

#include <glm/glm.hpp>
#include <glad/glad.h>

#include <vector>
#include <memory>

class OGLSphere {
protected:
	static int _resolution;
public:
	OGLSphere() = delete;
	virtual ~OGLSphere() = delete;

	static std::shared_ptr<Mesh> get_mesh(int res);
};

#endif /* OGLSPHERE_H_ */
