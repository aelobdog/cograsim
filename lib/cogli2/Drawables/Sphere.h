/*
 * Sphere.h
 *
 *  Created on: 30/ott/2013
 *      Author: lorenzo
 */

#ifndef SPHERE_H_
#define SPHERE_H_

#include "Shape.h"

class Sphere: public Shape {
protected:
	glm::vec4 _color;
	float _r;

public:
	Sphere(glm::vec3 pos, float r, glm::vec4 color);
	virtual ~Sphere();

	virtual void accept_drawer(Drawer &d);

	float r() { return _r; }
	glm::vec4 color() { return _color; }
	virtual void set_opacity(float n_op) { _set_color_opacity(_color, n_op); }
};

#endif /* SPHERE_H_ */
