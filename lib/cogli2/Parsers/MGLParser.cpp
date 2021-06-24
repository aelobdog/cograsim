/*
 * MGLParser.cpp
 *
 *  Created on: 20/ott/2013
 *      Author: lorenzo
 */

#include "MGLParser.h"
#include "../Utils/StringHelper.h"
#include "../Utils/Logging.h"
#include "../Drawables/Sphere.h"
#include "../Drawables/Cylinder.h"
#include "../Drawables/Icosahedron.h"
#include "../Drawables/Arrow.h"
#include "../Drawables/DHS.h"
#include "../Drawables/PatchyParticle.h"

using std::string;
using std::vector;
using glm::vec3;
using glm::vec4;

MGLParser::MGLParser(const std::queue<string> &input_files) : Parser(input_files) {

}

MGLParser::~MGLParser() {

}

Shape *MGLParser::_build_shape(vector<string> &spl) {
	Shape *res = NULL;

	string key;
	// which object do we need to build? The default is a sphere
	if(spl.size() == 6) key = "S";
	else {
		key = spl[6];
	}

	vec3 pos;
	if(!StringHelper::get_vec3(spl, 0, pos)) {
		throw string("Cannot cast coordinates to double (x=" + spl[0] + ", y=" + spl[1] + ", z=" + spl[2] + ")\n");
	}

	if(spl[3] != "@") {
		Logging::log_debug("There is no '@' after coordinates (found '" + spl[3] + "')\n");
		return NULL;
	}

	double r;
	if(!StringHelper::from_string<double>(spl[4], r)) {
		Logging::log_debug("Cannot cast radius to double (r=" + spl[4] + ")\n");
		return NULL;
	}

	vec4 color = _get_color(spl[5]);

	if(key == "S") {
		res = new Sphere(pos, r, color);
	}
	else if(key == "C") {
		vec3 axis;
		if(!StringHelper::get_vec3(spl, 7, axis)) {
			throw string("Cannot cast the components of the cylinder's axis to double (x=" + spl[7] + ", y=" + spl[8] + ", z=" + spl[9] + ")\n");
		}
		res = new Cylinder(pos, axis, r, color);
	}
	else if(key == "A") {
		vec3 axis;
		if(!StringHelper::get_vec3(spl, 7, axis)) {
			throw string("Cannot cast the components of the arrows's axis to double (x=" + spl[7] + ", y=" + spl[8] + ", z=" + spl[9] + ")\n");
		}

		double cyl_length;
		if(!StringHelper::from_string<double>(spl[10], cyl_length)) {
			Logging::log_debug("Cannot cast cylinder length to double (r=" + spl[10] + ")\n");
			return NULL;
		}

		res = new Arrow(pos, axis, r, cyl_length, color);
	}
	else if(key == "D") {
		vec3 dip;
		if(!StringHelper::get_vec3(spl, 7, dip)) {
			throw string("Cannot cast the components of the dipole to double (x=" + spl[7] + ", y=" + spl[8] + ", z=" + spl[9] + ")\n");
		}
		if(color[3] == 1.f) {
			Logging::log_warning("Dipoles must have opacity < 1.0, defaulting to 0.5\n");
			color[3] = 0.5f;
		}
		res = new DHS(pos, dip, r, color);
	}
	else if(key == "M") {
		PatchyParticle *p_res = new PatchyParticle(pos, r, color);
		if((spl.size()-7) % 5 != 0) {
			Logging::log_warning("Each patch should be given a relative position, a width and a colour\n");
			return NULL;
		}
		int N_patches = (spl.size() - 7) / 5;
		for(int i = 0; i < N_patches; i++) {
			int base_idx = i*5 + 7;
			glm::vec3 pos;
			float width;

			StringHelper::get_vec3(spl, base_idx, pos);
			StringHelper::from_string<float>(spl[base_idx + 3], width);
			glm::vec4 color = _get_color(spl[base_idx + 4]);
			p_res->add_patch(pos, width, color);
		}

		res = p_res;
	}
	else if (key == "I") {
		vec3 axis, axis2;
		if((!StringHelper::get_vec3(spl, 7, axis)) || (!StringHelper::get_vec3(spl,10,axis2))) {
			throw string("Cannot cast the components of the icosahedon's axis to double (x=" + spl[7] + ", y=" + spl[8] + ", z=" + spl[9] + ")\n");
		}
		res = new Icosahedron (pos, axis, axis2, r, color);
	}
	else throw StringHelper::sformat("Unrecognized particle identifier '%s'\n", key.c_str());

	return res;
}

Scene *MGLParser::next_scene() {
	Scene *my_scene = new Scene();

	try {
		_open_input_if_required();

		// set scene title
		std::stringstream title;
		title << _current_input_name << ": " << _conf_index;
		my_scene->set_base_title(title.str());

		// here we parse the first line that might be a header line that specifies the box
		int current = _current_input.tellg();
		string buff;
		getline(_current_input, buff);
		vector<string> spl = StringHelper::split(buff, ':');
		vec3 box;
		bool box_specified = true;

		// box can be expressed in two ways
		if(spl[0] == ".Box") {
			spl = StringHelper::split(spl[1], ',');
			if(spl.size() != 3) throw(_current_input_name + StringHelper::sformat(": malformed first line\n"));
			else {
				StringHelper::from_string<float>(spl[0], box[0]);
				StringHelper::from_string<float>(spl[1], box[1]);
				StringHelper::from_string<float>(spl[2], box[2]);
			}
		}
		else if(spl[0] == ".Vol") {
			double vol = 0.;
			StringHelper::from_string<double>(spl[1], vol);

			box[0] = box[1] = box[2] = pow(vol, 1./3.);
		}
		else {
			_current_input.seekg(current);
			box_specified = false;
		}

		int curr_line = 2;
		int curr_id = 0;
		bool done = false;
		vec3 min_pos(1e10, 1e10, 1e10);
		vec3 max_pos(-1e10, -1e10, -1e10);
		while(!done) {
			current = _current_input.tellg();
			getline(_current_input, buff);
			// if the first character is a full stop and the string contains a colon then we have reached the end of the
			// configuration. If this is the case, we have to be sure that the next time we read from this file, we get
			// the first line.
			vector<string> spl = StringHelper::split(buff, ':');
			if(buff[0] == '.' && spl.size() == 2) {
				_current_input.seekg(current);
				done = true;
			}
			else {
				spl = StringHelper::split(buff);
				// if the line is not empty or it does not start with a #
				if(spl.size() != 0 && spl[0][0] != '#') {
					if(spl.size() < 6) {
						throw string(_current_input_name + ": syntax error in line " + StringHelper::to_string<int>(curr_line) + "\n");
					}

					Shape *p = _build_shape(spl);
					if(p == NULL) {
						throw string(_current_input_name + StringHelper::sformat(": syntax error in line %d\n", curr_line));
					}

					p->set_id(curr_id);
					my_scene->add_shape(p);
					curr_id++;

					vec3 pos = p->position();
					for(int d = 0; d < 3; d++) {
						if(pos[d] < min_pos[d]) min_pos[d] = pos[d];
						if(pos[d] > max_pos[d]) max_pos[d] = pos[d];
					}
				}
			}
			curr_line++;
			if(_current_input.eof()) done = true;
		}

		if(!box_specified) {
			box = max_pos - min_pos;
			// we set a minimum box size
			if(glm::length(box) < 1.) box = vec3(5., 5., 5.);
		}

		my_scene->set_box(box);

		_close_current_input_if_eof();
	}
	catch (string &error) {
		Logging::log_critical(error);
		exit(1);
	}

	return my_scene;
}

glm::vec3 MGLParser::_get_box(string &line) {
	vector<string> spl = StringHelper::split(line, ':');
	vec3 box;
	bool done = true;

	if(spl.size() != 2) done = false;

	// box can be expressed in two ways
	if(spl[0] == ".Box") {
		spl = StringHelper::split(spl[1], ',');
		if(spl.size() != 3) done = false;
		else {
			StringHelper::from_string<float>(spl[0], box[0]);
			StringHelper::from_string<float>(spl[1], box[1]);
			StringHelper::from_string<float>(spl[2], box[2]);
		}
	}
	else if(spl[0] == ".Vol") {
		double vol = 0.;
		StringHelper::from_string<double>(spl[1], vol);

		box[0] = box[1] = box[2] = pow(vol, 1./3.);
	}
	else done = false;

	if(!done) throw(_current_input_name + StringHelper::sformat(": malformed first line\n"));

	return box;
}
