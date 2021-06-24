/*
 * Triangle.h
 *
 *  Created on: 30/ott/2013
 *      Author: lorenzo
 */

#ifndef TRIANGLE_H_
#define TRIANGLE_H_

#include "Shape.h"

#include <vector>

class Triangle: public Shape {
protected:
	glm::vec4 _color;
	std::vector<glm::vec3> _vertices;

public:
	Triangle(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec4 color);
	virtual ~Triangle();

	virtual void accept_drawer(Drawer &d);

	std::vector<glm::vec3> &vertices() { return _vertices; }
	glm::vec4 color() { return _color; }
	virtual void set_opacity(float n_op) { _set_color_opacity(_color, n_op); }
	virtual void shift(glm::vec3 &amount);
};

#endif /* TRIANGLE_H_ */
