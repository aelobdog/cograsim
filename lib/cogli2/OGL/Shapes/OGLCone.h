/*
 * OGLCone.h
 *
 *  Created on: 12/nov/2013
 *      Author: lorenzo
 */

#ifndef OGLCONE_H_
#define OGLCONE_H_

#include "Mesh.h"

#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <memory>

class OGLCone {
protected:
	static int _resolution;
public:
	OGLCone() = delete;
	virtual ~OGLCone() = delete;

	static std::shared_ptr<Mesh> get_mesh(int res, bool with_base=true);
};

#endif /* OGLCONE_H_ */
