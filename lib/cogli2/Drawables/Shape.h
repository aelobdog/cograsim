/*
 * Shape.h
 *
 *  Created on: 03/nov/2013
 *      Author: lorenzo
 */

#ifndef SHAPE_H_
#define SHAPE_H_

#include <glm/glm.hpp>
#include "Drawable.h"

class Shape : public Drawable {
protected:
	bool _is_pickable;
	bool _is_selected;
	bool _is_visible;
	glm::vec3 _position;

	virtual void _set_color_opacity(glm::vec4 &color, float n_op);

public:
	Shape(glm::vec3 pos=glm::vec3(0., 0., 0.));
	virtual ~Shape();

	virtual void set_selected(bool val) { _is_selected = val; }
	virtual void toggle_selected() { set_selected(!_is_selected); }
	virtual bool is_selected() const { return _is_selected; }

	virtual void set_pickable(bool val) { _is_pickable = val; }
	virtual bool is_pickable() const { return _is_pickable; }

	void set_visible(bool v) { _is_visible = v; }
	bool is_visible() const { return _is_visible; }
	virtual void set_opacity(float n_op) = 0;

	virtual glm::vec3 position() const { return _position; }
	virtual glm::mat3 inertia_tensor(glm::vec3);
	virtual void shift(glm::vec3 &amount) { _position += amount; }
	virtual void rotate(glm::mat3, glm::vec3);
};

#endif /* SHAPE_H_ */
