/*
 * Arrow.h
 *
 *  Created on: 22/ott/2013
 *      Author: lorenzo
 */

#ifndef ARROW_H_
#define ARROW_H_

#include "Cylinder.h"

class Arrow: public Shape {
protected:
	glm::vec3 _axis;
	float _r_cyl;
	float _cyl_length;
	float _cone_length;
	float _tot_length;
	glm::vec4 _color;

public:
	Arrow(glm::vec3 pos, glm::vec3 axis, float r_cyl, float cyl_length, glm::vec4 color);
	virtual ~Arrow();

	virtual void accept_drawer(Drawer &d);
	virtual glm::vec3 axis() { return _axis; }
	float r_cyl() { return _r_cyl; }
	float cyl_length() { return _cyl_length; }
	float cone_length() { return _cone_length; }
	glm::vec4 color() { return _color; }
	virtual void set_opacity(float n_op) { _set_color_opacity(_color, n_op); }
	virtual void rotate(glm::mat3, glm::vec3);
};

#endif /* ARROW_H_ */
