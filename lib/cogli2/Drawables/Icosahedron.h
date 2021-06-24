/*
 * Icosahedron.h
 *
 *  Created on: 16/lug/2018
 *      Author: flavio
 */

#ifndef ICOSAHEDRON_H_
#define ICOSAHEDRON_H_

#include "Shape.h"
#include <vector>

struct IcoPatch {
	glm::vec3 pos;
	float radius;
	glm::vec4 color;

	IcoPatch(glm::vec3 n_pos, float n_radius, glm::vec4 n_color);
	virtual ~IcoPatch();
};

class Icosahedron: public Shape {
protected:
	glm::vec3 _axis;
	glm::vec3 _axis2;
	float _r;
	glm::vec4 _color;
	
	std::vector<IcoPatch> _patches;
	
public:
	Icosahedron(glm::vec3 pos, glm::vec3 axis, glm::vec3 axis2,  float r, glm::vec4 color);
	virtual ~Icosahedron();

	void add_patch(glm::vec3 pos, float radius, glm::vec4 color);
	std::vector<IcoPatch> get_patches() { return _patches; }
	virtual void accept_drawer(Drawer &d);
	virtual glm::vec3 axis() { return _axis; }
	virtual glm::vec3 axis2() { return _axis2; }
	float r() { return _r; }
	glm::vec4 color() { return _color; }
	virtual void set_opacity(float n_op) { _set_color_opacity(_color, n_op); }
	virtual void rotate(glm::mat3, glm::vec3);
};

#endif /* ICOSAHEDRON_H_ */
