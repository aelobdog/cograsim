/*
 * Drawable.h
 *
 *  Created on: 20/ott/2013
 *      Author: lorenzo
 */

#ifndef DRAWABLE_H_
#define DRAWABLE_H_

class Drawer;

class Drawable {
protected:
	int _id;

public:
	Drawable();
	virtual ~Drawable();

	virtual void accept_drawer(Drawer &) = 0;

	void set_id(int id) { _id = id; }
	int get_id() { return _id; }
};

#endif /* DRAWABLE_H_ */
