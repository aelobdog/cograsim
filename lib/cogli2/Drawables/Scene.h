/*
 * Scene.h
 *
 *  Created on: 20/ott/2013
 *      Author: lorenzo
 */

#ifndef SCENE_H_
#define SCENE_H_

#include <vector>
#include <string>
#include <set>
#include <memory>

#include <glm/glm.hpp>
#include "../Actions/BaseAction.h"
#include "../Utils/StringHelper.h"
#include "Shape.h"

class Cylinder;

struct CentreOptions {
	bool always_centre;
	bool consider_inertia;
	int centre_on;

	CentreOptions() {
		always_centre = false;
		consider_inertia = false;
		centre_on = -1;
	}
};

class Scene: public Drawable {
protected:
	std::vector<Shape *> _shapes;
	std::vector<Shape *> _sortable_shapes;
	std::vector<std::shared_ptr<Cylinder>> _box;
	std::string _base_title;
	glm::vec3 _box_size;
	glm::vec3 _shift;

	std::vector<int> _select_groups[10];

	std::vector<std::shared_ptr<BaseAction>> _actions;
	unsigned int _current_action;

	/**
	 * @brief Number of shapes to be shifted together
	 *
	 * This is used when the system contains constructs made up of
	 * multiple shapes. One example would be DNA tetramers. It
	 * defaults to 1.
	 */
	static int _shift_every;
	static bool _show_box;

	bool _dirty = true;

public:
	static glm::vec3 global_shift;

	Scene();
	virtual ~Scene();

	virtual void accept_drawer(Drawer &);

	void add_shape(Shape *);

	std::string get_title();
	void set_base_title(std::string nt) {
		_base_title = nt;
	}

	static void set_shift_every(int s) {
		_shift_every = s;
	}
	static int get_shift_every() {
		return _shift_every;
	}

	void hide_selected();
	void show_all_shapes();
	void select_all_shapes();
	void invert_selection();
	void select_shape(int id);
	void set_selected_as_group(int g);
	void select_group(int g);

	const std::vector<Shape *> &get_shapes() {
		return _shapes;
	}

	std::vector<Shape *> &get_sortable_shapes() {
		return _sortable_shapes;
	}

	std::vector<std::shared_ptr<Cylinder>> &get_box() {
		return _box;
	}

	glm::vec3 get_box_size() {
		return _box_size;
	}

	void set_box(glm::vec3 b);
	void toggle_box();
	bool is_box_visible() {
		return Scene::_show_box;
	}

	void bring_in_box();
	void translate(glm::vec3 amount, bool is_global = true);
	void centre(CentreOptions &);
	void align_according_to_inertia_tensor(glm::vec3);

	glm::vec3 get_centre_location();

	/**
	 * Make it so that this Scene has the same appearance as all the other ones.
	 * This comprises the update of this Scene's shift to match the global one.
	 */
	void normalise();

	static std::string get_state();
	std::string print_control_groups();
	static void load_state_from_file(std::ifstream &in);
	void load_groups_from_file(std::ifstream &in);

	void set_actions(const std::vector<std::shared_ptr<BaseAction>> &actions);
	bool next_action();
	bool previous_action();

	bool dirty();
	void set_dirty(bool);
};

#endif /* SCENE_H_ */
