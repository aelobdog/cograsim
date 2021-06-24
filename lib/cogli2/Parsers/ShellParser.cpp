/*
 * ShellParser.cpp
 *
 *  Created on: 1 Mar 2018
 *      Author: lorenzo
 */

#include "ShellParser.h"

#include "../Utils/StringHelper.h"
#include "../Utils/Logging.h"
#include "../Drawables/Triangle.h"

#include <set>

using std::string;
using std::vector;
using std::set;
using glm::vec3;
using glm::vec4;

ShellParser::ShellParser(string topology, const std::queue<string> &input_files) :
				Parser(input_files),
				_topology_filename(topology) {

}

ShellParser::~ShellParser() {

}

Scene *ShellParser::next_scene() {
	Scene *my_scene = new Scene();

	try {
		_open_input_if_required();

		// set scene title
		std::stringstream title;
		title << _current_input_name << ": " << _conf_index;
		my_scene->set_base_title(title.str());

		string buff;
		getline(_current_input, buff);
		getline(_current_input, buff);
		int N;
		StringHelper::from_string<int>(buff, N);

		getline(_current_input, buff);
		vector<string> spl = StringHelper::split(buff);
		if(spl[0] == "cube") {
			double L;
			StringHelper::from_string<double>(spl[1], L);
			vec3 box(L, L, L);
			my_scene->set_box(box);
		}
		else if(spl[0] == "cuboid") {
			double Lx, Ly, Lz;
			StringHelper::from_string<double>(spl[1], Lx);
			StringHelper::from_string<double>(spl[2], Ly);
			StringHelper::from_string<double>(spl[3], Lz);
			vec3 box(Lx, Ly, Lz);
			my_scene->set_box(box);
		}
		else {
			throw string("Cannot parse the box line '" + buff + "'\n");
		}

		vector<vec3> poss;
		for(int i = 0; i < N; i++) {
			getline(_current_input, buff);

			spl = StringHelper::split(buff);
			vec3 pos;
			if(!StringHelper::get_vec3(spl, 0, pos)) {
				throw string("Cannot cast coordinates to double (x=" + spl[0] + ", y=" + spl[1] + ", z=" + spl[2] + ")\n");
			}
			poss.push_back(pos);
		}

		_close_current_input_if_eof();

		std::ifstream topology(_topology_filename.c_str());
		if(!topology.good()) {
			throw string("The topology file '" + _topology_filename + "' is not readable");
		}

		int curr_id = 0;
		set<vector<int> > triangles;
		while(topology.good()) {
			getline(topology, buff);
			spl = StringHelper::split(buff, ' ');
			if(spl[0] == "dihedral") {
				// TODO: all of this can be written way better
				int v1, v2, v3, v4;
				StringHelper::from_string<int>(spl[2], v1);
				StringHelper::from_string<int>(spl[3], v2);
				StringHelper::from_string<int>(spl[4], v3);
				StringHelper::from_string<int>(spl[5], v4);

				vector<int> new_triangle;
				new_triangle.push_back(v1);
				new_triangle.push_back(v2);
				new_triangle.push_back(v3);
				std::sort(new_triangle.begin(), new_triangle.end());
				triangles.insert(new_triangle);

				new_triangle.clear();
				new_triangle.push_back(v2);
				new_triangle.push_back(v3);
				new_triangle.push_back(v4);
				std::sort(new_triangle.begin(), new_triangle.end());
				triangles.insert(new_triangle);
			}
		}

		for(set<vector<int> >::iterator it = triangles.begin(); it != triangles.end(); it++) {
			vec3 pos_1 = poss[(*it)[0]];
			vec3 pos_2 = poss[(*it)[1]];
			vec3 pos_3 = poss[(*it)[2]];
			Triangle *t = new Triangle(pos_1, pos_2, pos_3, vec4(1, 0, 0, 1));
			t->set_id(curr_id);
			curr_id++;
			my_scene->add_shape(t);
		}
	} catch (string &error) {
		Logging::log_critical(error);
		exit(1);
	}

	return my_scene;
}
