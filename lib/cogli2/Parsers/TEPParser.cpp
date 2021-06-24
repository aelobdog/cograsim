/*
 * TEPParser.cpp
 *
 *  Created on: 18/jun/2016
 *      Author: lorenzo
 */

#include "TEPParser.h"
#include "../Utils/StringHelper.h"
#include "../Utils/Logging.h"
#include "../Drawables/Sphere.h"
#include "../Drawables/Group.h"
#include "../cl_arguments.h"

using std::string;
using std::vector;
using glm::vec3;
using glm::vec4;

TEPParser::TEPParser(string topology, const std::queue<string> &input_files) : Parser(input_files), _N(-1) {
	_read_topology(topology);
	_twist_threshold = 0.96;
	_twist_b = 0.95;
}

TEPParser::~TEPParser() {

}

void TEPParser::set_options(option::Option *options) {
	Parser::set_options(options);

	if(options[TEP_THRESHOLD]) {
		_twist_threshold = atof(options[TEP_THRESHOLD].arg);
		Logging::log_info("Using TEP threshold = %f\n", _twist_threshold);
	}
}

void TEPParser::_set_color_from_HSV(glm::vec4 &color, float h, float s, float v) {
	if (h < 0) h = 0;
	if (h > 359) h = 359;
	if (s < 0) s = 0;
	if (s > 1) s = 100;
	if (v < 0) v = 0;
	if (v > 1) v = 100;

	float tmp = h / 60.0;
	int hi = floor(tmp);
	float f = tmp - hi;
	float p = v * (1 - s);
	float q = v * (1 - f * s);
	float t = v * (1 - (1 - f) * s);

	switch (hi) {
	case 0:
		color[0] = v;
		color[1] = t;
		color[2] = p;
		break;
	case 1:
		color[0] = q;
		color[1] = v;
		color[2] = p;
		break;
	case 2:
		color[0] = p;
		color[1] = v;
		color[2] = t;
		break;
	case 3:
		color[0] = p;
		color[1] = q;
		color[2] = v;
		break;
	case 4:
		color[0] = t;
		color[1] = p;
		color[2] = v;
		break;
	case 5:
		color[0] = v;
		color[1] = p;
		color[2] = q;
		break;
	}
}

Scene *TEPParser::next_scene() {
	Scene *my_scene = new Scene();

	try {
		_open_input_if_required();

		_parse_headers(_current_input, my_scene);

		// set scene title
		std::stringstream title;
		title << _current_input_name << ": " << _conf_index << ", time: " << _time;
		my_scene->set_base_title(title.str());


		int curr_line = 3;
		bool done = false;
		int index = 0;
		glm::vec3 centre_pos(0.f, 0.f, 0.f);
		vec3 old_v1, old_v2, old_v3;
		while(!done) {
			string buff;
			int current = _current_input.tellg();
			getline(_current_input, buff);
			// if the first character is a 't' and the string contains a has 3
			// entries we have reached the end of the configuration. If this is
			// the case, we have to be sure that the next time we read from this
			// file, we get the first line.
			vector<string> spl = StringHelper::split(buff);
			if(buff[0] == 't' && spl.size() == 3) {
				_current_input.seekg(current);
				done = true;
			}
			else if(spl.size() != 0){
				spl = StringHelper::split(buff);
				vec3 pos, v1, v3;

				StringHelper::get_vec3(spl, 0, pos);
				StringHelper::get_vec3(spl, 3, v1);
				StringHelper::get_vec3(spl, 6, v3);

				vec3 v2 = glm::cross(v3, v1);
				glm::vec4 color = glm::vec4(1.f, 0.f, 0.f, 1.f);
				if(index > 0) {
					float M = glm::dot(v2, old_v2) + glm::dot(v3, old_v3);
					float L = 1 + glm::dot(v1, old_v1);
					float cos_alpha_plus_gamma = M/L;
					// black
					if(-cos_alpha_plus_gamma >= _twist_b) color = glm::vec4(0.f, 0.f, 0.f, 1.f);
					// blue
					if(cos_alpha_plus_gamma < _twist_threshold) color = glm::vec4(0.f, 0.f, 1.f, 1.f);
					else {
						// this is the number (which lies in the [0, 1] interval) which will be used to interpolate colours
						float norm_angle = (cos_alpha_plus_gamma - _twist_threshold) / (1. - _twist_threshold);

						// in degrees, 0 is red, 120 is green, 240 is blue
						float h = 240 + norm_angle*120;
						float s = 0.9;
						float v = 0.9;
						_set_color_from_HSV(color, h, s, v);
					}
				}
				//previous color glm::vec4(50.f/255, 99.f/255, 66.f/255, 1.f)
//				Shape *p = new Sphere(pos, 0.5f, _base_colors["GREEN"]);
//				Shape *pp = new Sphere(pos + 0.5f*v3, 0.25f, color);
//				my_scene->add_shape(pp);
//				p->set_id(index);
//				my_scene->add_shape(p);
				Group *new_group = new Group();
				new_group->add_shape(new Sphere(pos, 0.5f, _base_colors["GREEN"]));
				new_group->add_shape(new Sphere(pos + 0.5f*v3, 0.25f, color));
				new_group->set_id(index);
				my_scene->add_shape(new_group);
				index++;

				old_v1 = v1;
				old_v2 = v2;
				old_v3 = v3;
			}
			curr_line++;
			if(_current_input.eof()) done = true;
		}

		if(_N != index) {
			Logging::log_critical("The number of nucleotides found in the trajectory (%d) is different from the one found in the topology (%d)\n", index, _N);
			exit(1);
		}

		_close_current_input_if_eof();
	}
	catch (string &error) {
		Logging::log_critical(error);
		exit(1);
	}

	return my_scene;
}

void TEPParser::_read_topology(std::string filename) {
		_current_input.open(filename.c_str());

		string buff;
		getline(_current_input, buff);
		std::vector<std::string> spl = StringHelper::split(buff);
		if(spl.size() != 2) {
			Logging::log_critical("Invalid topology found in '%s'\n", filename.c_str());
			exit(1);
		}

		StringHelper::from_string<int>(spl[0], _N);

		_current_input.close();
}
