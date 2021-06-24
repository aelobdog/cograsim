/*
 * OGLIcosahedron.h
 *
 *  Created on: 03/nov/2013
 *      Author: Flavio 
 */

#ifndef OGLICOSAHEDRON_H_
#define OGLICOSAHEDRON_H_

#include "Mesh.h"

#include <glm/glm.hpp>
#include <glad/glad.h>

#include <vector>
#include <memory>

class OGLIcosahedron {
public:
	OGLIcosahedron() = delete;
	virtual ~OGLIcosahedron() = delete;

	static std::shared_ptr<Mesh> get_mesh();
};

#endif /* OGLICOSAHEDRON_H_ */
