/*
 * Scene.cpp
 *
 *  Created on: 20/ott/2013
 *      Author: lorenzo
 */

#include "Scene.h"

#include "Cylinder.h"
#include "../Drawers/Drawer.h"
#include "../Utils/StringHelper.h"
#include "../Utils/Logging.h"
#include "../Utils/MathHelper.h"

#include <sstream>
#include <fstream>

using namespace std;
using glm::vec3;

glm::vec3 Scene::global_shift(0.f, 0.f, 0.f);
bool Scene::_show_box = true;
int Scene::_shift_every = 1;

Scene::Scene() :
				Drawable(),
				_shift(0.f, 0.f, 0.f) {
	_current_action = 0;
}

Scene::~Scene() {
	for(vector<Shape *>::iterator it = _shapes.begin(); it != _shapes.end(); it++) {
		delete *it;
	}
}

void Scene::accept_drawer(Drawer &d) {
	d.visit(*this);
}

void Scene::add_shape(Shape *n_o) {
	set_dirty(true);

	_shapes.push_back(n_o);
	_sortable_shapes.push_back(n_o);
}

void Scene::set_actions(const std::vector<std::shared_ptr<BaseAction>> &actions) {
	_actions = actions;
	_current_action = 0;
}

std::string Scene::get_title() {
	string title = _base_title;
	if(_current_action > 0) {
		title += StringHelper::sformat(", action %u", _current_action);
	}
	return title;
}

bool Scene::previous_action() {
	set_dirty(true);

	if(_actions.size() == 0) {
		return false;
	}
	// if we are at the very beginning there is nothing we need to do
	if(_current_action == 0) {
		return false;
	}

	_current_action--;
	_actions[_current_action]->undo_action();

	return true;
}

bool Scene::next_action() {
	set_dirty(true);

	if(_actions.size() == 0) {
		return false;
	}
	// if the previous "current" action was the last one, then there is nothing to do
	if(_current_action == _actions.size()) {
		return false;
	}

	_actions[_current_action]->do_action();
	_current_action++;

	return true;
}

void Scene::hide_selected() {
	set_dirty(true);

	for(vector<Shape *>::iterator it = _shapes.begin(); it != _shapes.end(); it++) {
		if((*it)->is_selected()) {
			(*it)->set_visible(false);
		}
	}
}

void Scene::show_all_shapes() {
	set_dirty(true);

	for(vector<Shape *>::iterator it = _shapes.begin(); it != _shapes.end(); it++) {
		(*it)->set_visible(true);
	}
}

void Scene::select_all_shapes() {
	set_dirty(true);

	for(vector<Shape *>::iterator it = _shapes.begin(); it != _shapes.end(); it++) {
		(*it)->set_selected(true);
	}
}

void Scene::invert_selection() {
	set_dirty(true);

	for(vector<Shape *>::iterator it = _shapes.begin(); it != _shapes.end(); it++) {
		(*it)->toggle_selected();
	}
}

void Scene::select_shape(int id) {
	if(id < 0 || id > (int) _shapes.size()) {
		Logging::log_warning("The shape n. %d does not exist: it cannot be select\n", id);
	}
	else {
		set_dirty(true);
		_shapes[id]->toggle_selected();
	}
}

void Scene::set_selected_as_group(int g) {
	if(g < 0 || g > 9) {
		Logging::log_warning("The select group %d does not exist\n", g);
		return;
	}

	_select_groups[g].clear();
	int cntr = 0;
	for(vector<Shape *>::iterator it = _shapes.begin(); it != _shapes.end(); it++) {
		if((*it)->is_selected()) _select_groups[g].push_back(cntr);
		cntr++;
	}
}

void Scene::select_group(int g) {
	if(g < 0 || g > 9) {
		Logging::log_warning("The select group %d does not exist\n", g);
		return;
	}

	for(vector<Shape *>::iterator it = _shapes.begin(); it != _shapes.end(); it++) {
		(*it)->set_selected(false);
	}

	for(vector<int>::iterator it = _select_groups[g].begin(); it != _select_groups[g].end(); it++) {
		_shapes[(*it)]->set_selected(true);
	}
}

void Scene::bring_in_box() {
	vec3 d = vec3(0.f, 0.f, 0.f);
	translate(d);
}

void Scene::set_box(glm::vec3 b) {
	// since set_box can be called more than once, we have to first clean up the _box vector
	_box.clear();

	_box_size = b;

	glm::vec4 bc(0.f, 0.f, 0.f, 1.f);
	float r = b[0] / 200.f;

	_box.push_back(std::make_shared<Cylinder>(vec3(0.f, 0.f, 0.f), vec3(b[0], 0.f, 0.f), r, bc));
	_box.push_back(std::make_shared<Cylinder>(vec3(0.f, 0.f, 0.f), vec3(0.f, b[1], 0.f), r, bc));
	_box.push_back(std::make_shared<Cylinder>(vec3(0.f, 0.f, 0.f), vec3(0.f, 0.f, b[2]), r, bc));

	_box.push_back(std::make_shared<Cylinder>(b, vec3(-b[0], 0.f, 0.f), r, bc));
	_box.push_back(std::make_shared<Cylinder>(b, vec3(0.f, -b[1], 0.f), r, bc));
	_box.push_back(std::make_shared<Cylinder>(b, vec3(0.f, 0.f, -b[2]), r, bc));

	_box.push_back(std::make_shared<Cylinder>(vec3(0.f, 0.f, b[2]), vec3(0.f, b[1], 0.f), r, bc));
	_box.push_back(std::make_shared<Cylinder>(vec3(0.f, 0.f, b[2]), vec3(b[0], 0.f, 0.f), r, bc));

	_box.push_back(std::make_shared<Cylinder>(vec3(b[0], b[1], 0.f), vec3(-b[0], 0.f, 0.f), r, bc));
	_box.push_back(std::make_shared<Cylinder>(vec3(b[0], b[1], 0.f), vec3(0.f, -b[1], 0.f), r, bc));

	_box.push_back(std::make_shared<Cylinder>(vec3(b[0], 0.f, b[2]), vec3(0.f, 0.f, -b[2]), r, bc));

	_box.push_back(std::make_shared<Cylinder>(vec3(0.f, b[1], 0.f), vec3(0.f, 0.f, b[2]), r, bc));
}

void Scene::toggle_box() {
	set_dirty(true);

	Scene::_show_box = !Scene::_show_box;
}

void Scene::translate(vec3 amount, bool is_global) {
	set_dirty(true);

	if(is_global) {
		Scene::global_shift += amount;
	}
	_shift += amount;

	vec3 n_pos(0.f, 0.f, 0.f);
	int count = 1;
	for(auto it = _shapes.begin(); it != _shapes.end(); it++) {
		Shape *s = *it;
		n_pos += s->position() + amount;

		// if _shift_every = 1 this is just the shifting and PBC wrapping applied to
		// a single particle. If _shift_every > 1, the PBC wrapping is computed with
		// respect to the centre of mass of the group of particles of size _shift_every
		if(count % _shift_every == 0) {
			n_pos /= (float) _shift_every;
			vec3 diff = amount - _box_size * glm::floor(n_pos / _box_size);
			// we have to shift the current shape and the (_shift_every-1) previous ones
			vector<Shape *>::iterator s_it = it;
			for(int i = _shift_every; i > 0; s_it--, i--) {
				(*s_it)->shift(diff);
			}

			n_pos = vec3(0.f, 0.f, 0.f);
		}
		count++;
	}
}

void Scene::normalise() {
	translate(Scene::global_shift - _shift, false);
}

void Scene::centre(CentreOptions &centre) {
	vec3 wrt(0.f, 0.f, 0.f);

	vec3 new_com = _box_size * 0.5f;
	if(centre.centre_on > -1) {
		if(centre.centre_on > (int) _shapes.size()) {
			Logging::log_critical("The ID passed as the argument of --always-centre is invalid\n");
			exit(1);
		}
		wrt = _shapes[centre.centre_on]->position();
	}
	else {
		int visible_shapes = 0;
		for(auto shape : _shapes) {
			if(shape->is_visible()) {
				wrt += shape->position();
				visible_shapes++;
			}
		}
		wrt /= visible_shapes;
	}
	vec3 my_shift = -wrt + new_com;

	translate(my_shift);
	if(centre.consider_inertia) {
		align_according_to_inertia_tensor(new_com);
	}
}

void Scene::align_according_to_inertia_tensor(vec3 wrt) {
	set_dirty(true);

	glm::mat3 inertia_tensor(0.f);
	glm::mat3 identity(1.f);

	for(vector<Shape *>::iterator it = _shapes.begin(); it != _shapes.end(); it++) {
		Shape *s = *it;
		if(s->is_visible()) inertia_tensor += s->inertia_tensor(wrt);
	}

	vec3 i_moments;
	glm::mat3 i_axes;
	MathHelper::diagonalize_3x3(inertia_tensor, i_moments, i_axes);

	// enforce right handedness
	i_axes[2] = glm::cross(i_axes[0], i_axes[1]);
	glm::mat3 R = glm::transpose(i_axes);

	for(auto shape : _shapes) {
		vec3 shift = R * (shape->position() - wrt) - shape->position() + wrt;
		shape->shift(shift);
		shape->rotate(R, shape->position());
	}
}

glm::vec3 Scene::get_centre_location() {
	return _box_size / 2.f;
}

string Scene::get_state() {
	std::stringstream out;

	out << "visible_box " << Scene::_show_box << endl;
	out << "shift " << Scene::global_shift[0] << " " << Scene::global_shift[1] << " " << Scene::global_shift[2] << endl;

	return out.str();
}

string Scene::print_control_groups() {
	std::stringstream out;

	for(int i = 0; i < 10; i++) {
		if(_select_groups[i].size() > 0) {
			out << i << " - ";
			for(int k = 0; k < (int) _select_groups[i].size(); k++) {
				out << _select_groups[i][k] << " ";
			}
			out << endl;
		}
	}

	return out.str();
}

void Scene::load_groups_from_file(std::ifstream &in) {
	in.clear();
	in.seekg(0);

	for(int i = 0; i < (int) _select_groups[i].size(); i++) {
		_select_groups[i].clear();
	}

	while(in.good()) {
		std::string line;
		getline(in, line);
		std::vector<std::string> words = StringHelper::split(line);
		if(words.size() > 2 && line[0] != '#') {
			if(words[1] == "-") {
				int groupno = atoi(words[0].c_str());
				if(groupno < 0 || groupno > 9) {
					Logging::log_warning("The select group %d found in groupfile does not exist\n", groupno);
				}
				else {
					for(int j = 2; j < (int) words.size(); j++) {
						int tmpi = atoi(words[j].c_str());
						if(tmpi >= (int) _shapes.size()) Logging::log_warning("The particle found in groupfile %d does not exist\n", tmpi);
						else _select_groups[groupno].push_back(tmpi);
					}
				}
			}
		}
	}
	return;
}

void Scene::load_state_from_file(std::ifstream &in) {
	in.clear();
	in.seekg(0);
	while(in.good()) {
		std::string buff;
		getline(in, buff);

		std::vector<std::string> spl = StringHelper::split(buff);
		if(spl.size() > 0 && buff[0] != '#') {
			if(spl[0] == "visible_box") {
				StringHelper::from_string<bool>(spl[1], Scene::_show_box);
			}
			else if(spl[0] == "shift") {
				StringHelper::get_vec3(spl, 1, Scene::global_shift);
			}
		}
	}
}

bool Scene::dirty() {
	return _dirty;
}

void Scene::set_dirty(bool d) {
	_dirty = d;
}
