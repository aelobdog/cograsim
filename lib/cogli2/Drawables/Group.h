/*
 * Group.h
 *
 *  Created on: 13 May 2016
 *      Author: rovigattil
 */

#ifndef DRAWABLES_GROUP_H_
#define DRAWABLES_GROUP_H_

#include <vector>

#include "Shape.h"

class Group: public Shape {
protected:
	glm::vec3 _com;
	std::vector<Shape *> _shapes;

public:
	Group();
	virtual ~Group();

	virtual void accept_drawer(Drawer &d);
	virtual glm::vec3 position() const { return _com/(float)_shapes.size(); }
	virtual void set_opacity(float n_op);

	void add_shape(Shape *s);
	virtual glm::mat3 inertia_tensor(glm::vec3);
	virtual void shift(glm::vec3 &amount);
	virtual void rotate(glm::mat3, glm::vec3);
};

#endif /* DRAWABLES_GROUP_H_ */
