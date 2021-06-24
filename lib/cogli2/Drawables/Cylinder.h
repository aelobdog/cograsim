/*
 * Cylinder.h
 *
 *  Created on: 22/ott/2013
 *      Author: lorenzo
 */

#ifndef CYLINDER_H_
#define CYLINDER_H_

#include "Shape.h"

class Cylinder: public Shape {
protected:
	glm::vec3 _axis;
	float _r;
	glm::vec4 _color;

public:
	Cylinder(glm::vec3 pos, glm::vec3 axis, float r, glm::vec4 color);
	virtual ~Cylinder();

	virtual void accept_drawer(Drawer &d);
	virtual glm::vec3 axis() { return _axis; }
	float r() { return _r; }
	glm::vec4 color() { return _color; }
	virtual void set_opacity(float n_op) { _set_color_opacity(_color, n_op); }
	virtual void rotate(glm::mat3, glm::vec3);
};

#endif /* CYLINDER_H_ */
