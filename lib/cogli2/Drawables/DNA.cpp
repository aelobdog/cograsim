/*
 * DNAStrand.cpp
 *
 *  Created on: 14/nov/2013
 *      Author: lorenzo
 */

#include "DNA.h"
#include "../Drawers/Drawer.h"
#include "../Utils/Logging.h"

int Nucleotide::mode = Nucleotide::DEFAULT;
int Nucleotide::base_color_mode = Nucleotide::DEFAULT_COLOUR;

using glm::vec3;
using std::map;

std::vector<glm::vec4> Nucleotide::_drums_colors;

Nucleotide::Nucleotide(glm::vec3 pos, glm::vec3 principal, glm::vec3 stacking, glm::vec4 color, std::string base_id) :
				Shape(pos) {
	_position = pos;
	_principal_axis = principal;
	_stacking_axis = stacking;
	_third_axis = glm::cross(_stacking_axis, _principal_axis);
	_color = color;
	_base_color = glm::vec4(0, 1.f, 1.f, 1.f);
	_base_id = base_id;
	_nid = -1;
	// horrible way of initialising static vectors
	if(_drums_colors.size() == 0) {
		// the bases are coloured according to the DRuMS color scheme (almost)
		// see https://www.umass.edu/molvis/drums/rationale/r_nabase.html
		// adenine (1th-element) is azure
		_drums_colors.push_back(glm::vec4(.3, .3, 1, 1));
		// guanine (2th-element) is green 
		_drums_colors.push_back(glm::vec4(.4, 1, .4, 1));
		// cytosine (3th-element) is carmine 
		_drums_colors.push_back(glm::vec4(1, .3, .3, 1));
		if(mode != RNA)
			// thymine (4th-element) is tweety yellow
			_drums_colors.push_back(glm::vec4(0.99, 0.99, 0.3, 1));
		else
			// uracil (also 4th-element) is ``umber brown'', which is actually a strong orange #b8860b
			_drums_colors.push_back(glm::vec4(.722, .525, .430, 1));
	}
}

Nucleotide::~Nucleotide() {

}

void Nucleotide::set_color(glm::vec4 c) {
	_color = c;
	if(base_color_mode == Nucleotide::BACKBONE_COLOUR) {
		_base_color = c;
	}
	else if(base_color_mode == Nucleotide::DRUMS_COLOUR) {
		_base_color = drums_base_color();
	}
}

void Nucleotide::accept_drawer(Drawer &d) {
	d.visit(*this);
}

void Nucleotide::set_orientation(glm::mat3 &m) {
	glm::mat3 mT = glm::transpose(m);
	_principal_axis = mT[0];
	_stacking_axis = mT[2];
	_third_axis = glm::cross(_stacking_axis, _principal_axis);
}

void Nucleotide::set_opacity(float n_op) {
	_set_color_opacity(_base_color, n_op);
	_set_color_opacity(_color, n_op);
}

void Nucleotide::rotate(glm::mat3 R, glm::vec3 wrt) {
	Shape::rotate(R, wrt);
	_principal_axis = R * _principal_axis;
	_stacking_axis = R * _stacking_axis;
	_third_axis = glm::cross(_stacking_axis, _principal_axis);
}

glm::vec3 Nucleotide::backbone() {
	switch(mode) {
	case MM_GROOVING:
		return _principal_axis * (-0.34f) + _third_axis * 0.34f;
	case RNA:
		return _principal_axis * (-0.4f) + _third_axis * (0.0f) + _stacking_axis * (0.2f);
	default:
		return _principal_axis * (-0.4f);
	}
}

glm::vec4 Nucleotide::drums_base_color() {
	// If it's a known character, return the relative drums color
	if(_base_id == "A") {
		return _drums_colors[0];
	}
	if(_base_id == "G") {
		return _drums_colors[1];
	}
	if(_base_id == "C") {
		return _drums_colors[2];
	}
	if(_base_id == "T" or _base_id == "U") {
		return _drums_colors[3];
	}
	int id = atoi(_base_id.c_str());
	// otherwise, assume it's a number and give it a color depending on it's index % 4
	if(id > 0) {
		return _drums_colors[id % 4];
	}
	// if it's negative, the color will be chosen in order to preserve complementarity
	else {
		return _drums_colors[3 - (3 - id) % 4];
	}
}

std::vector<glm::vec4> DNAStrand::_automatic_colors;

DNAStrand::DNAStrand() {
	_is_circular = false;
	_id = -1;

	// horrible way of initialising static vectors
	if(_automatic_colors.size() == 0) {
		_automatic_colors.push_back(glm::vec4(1, 0, 0, 1));
		_automatic_colors.push_back(glm::vec4(0, 1, 0, 1));
		_automatic_colors.push_back(glm::vec4(0, 0, 1, 1));

		_automatic_colors.push_back(glm::vec4(1, 1, 0, 1));
		_automatic_colors.push_back(glm::vec4(0, 1, 1, 1));
		_automatic_colors.push_back(glm::vec4(1, 0, 1, 1));

		_automatic_colors.push_back(glm::vec4(1, 0.6, 0, 1));

		_automatic_colors.push_back(glm::vec4(0.56f, 0.f, 1.f, 1.f));
		_automatic_colors.push_back(glm::vec4(0.647f, 0.165f, 0.165f, 1.f));
		_automatic_colors.push_back(glm::vec4(0.98f, 0.855f, 0.867f, 1.f));
	}
}

DNAStrand::~DNAStrand() {
	for(map<int, Nucleotide*>::iterator it = _nucleotides.begin(); it != _nucleotides.end(); it++) {
		delete it->second;
	}
}

void DNAStrand::accept_drawer(Drawer &d) {
	d.visit(*this);
}

void DNAStrand::add_nucleotide(Nucleotide *n, int nid) {
	_nucleotides[nid] = n;
	n->set_id(_id);
	n->set_nid(nid);
	_com += n->position();
}

void DNAStrand::set_color(glm::vec4 color) {
	for(map<int, Nucleotide*>::iterator it = _nucleotides.begin(); it != _nucleotides.end(); it++) {
		it->second->set_color(color);
	}
}

void DNAStrand::set_opacity(float n_op) {
	for(map<int, Nucleotide*>::iterator it = _nucleotides.begin(); it != _nucleotides.end(); it++) {
		it->second->set_opacity(n_op);
	}
}

void DNAStrand::set_automatic_color() {
	if(_id == -1) {
		Logging::log_warning("You have to provide DNAStrand with an id before setting automatic colors\n");
	}
	else {
		int n_colors = _automatic_colors.size();
		glm::vec4 color = _automatic_colors[(_id / Scene::get_shift_every()) % n_colors];
		set_color(color);
	}
}

void DNAStrand::finalise() {
	_com /= _nucleotides.size();
}

glm::vec3 DNAStrand::position() const {
	return _com;
}

void DNAStrand::shift(glm::vec3 &amount) {
	for(map<int, Nucleotide*>::iterator it = _nucleotides.begin(); it != _nucleotides.end(); it++) {
		it->second->shift(amount);
	}
	_com += amount;
}

void DNAStrand::rotate(glm::mat3 R, glm::vec3 wrt) {
	for(map<int, Nucleotide*>::iterator it = _nucleotides.begin(); it != _nucleotides.end(); it++) {
		it->second->rotate(R, wrt);
	}
}

glm::mat3 DNAStrand::inertia_tensor(glm::vec3 wrt) {
	glm::mat3 inertia(0.f);
	for(map<int, Nucleotide*>::iterator it = _nucleotides.begin(); it != _nucleotides.end(); it++) {
		inertia += it->second->inertia_tensor(wrt);
	}
	return inertia;
}
