/*
 * RBCParser.cpp
 *
 *  Created on: 20/ott/2013
 *      Author: lorenzo
 */

#include "RBCParser.h"
#include "../Utils/StringHelper.h"
#include "../Utils/Logging.h"
#include "../Drawables/Triangle.h"
#include "../Drawables/Sphere.h"

using std::string;
using std::vector;
using glm::vec3;
using glm::vec4;

RBCParser::RBCParser(const std::queue<string> &input_files) : Parser(input_files) {

}

RBCParser::~RBCParser() {

}

Scene *RBCParser::next_scene() {
	Scene *my_scene = new Scene();

	try {
		_open_input_if_required();

		// set scene title
		std::stringstream title;
		title << _current_input_name << ": " << _conf_index;
		my_scene->set_base_title(title.str());

		string buff;
		getline(_current_input, buff);

		int N_mesh, N_dpd;
		vector<string> spl = StringHelper::split(buff);
		StringHelper::from_string<int>(spl[0], N_mesh);
		StringHelper::from_string<int>(spl[1], N_dpd);
		int N = N_mesh + N_dpd;

		vector<RBCParticle> particles(N_mesh);

		vec3 max_pos(-1e5, -1e5, -1e5), min_pos(1e5, 1e5, 1e5);

		int curr_line = 2;
		int curr_id = 0;
		bool done = false;
		while(!done) {
			getline(_current_input, buff);

			spl = StringHelper::split(buff);
			if(curr_id < N_mesh) {
				vec3 pos;
				if(!StringHelper::get_vec3(spl, 0, pos)) {
					throw string("Cannot cast coordinates to double (x=" + spl[0] + ", y=" + spl[1] + ", z=" + spl[2] + ")\n");
				}
				particles[curr_id].set_pos(pos);
				particles[curr_id].set_index(curr_id);
				for(int i = 0; i < 3; i++) {
					if(pos[i] > max_pos[i]) max_pos[i] = pos[i];
					if(pos[i] < min_pos[i]) min_pos[i] = pos[i];
				}
			}
			// we have decided not to show DPD particles
			else if(curr_id < N) {
				vec3 pos;
				if(!StringHelper::get_vec3(spl, 0, pos)) {
					throw string("Cannot cast coordinates to double (x=" + spl[0] + ", y=" + spl[1] + ", z=" + spl[2] + ")\n");
				}
//				Sphere *s = new Sphere(pos, 0.1, vec4(1, 0, 0, 0.5));
//				s->set_id(curr_id);
//				my_scene->add_shape(s);
			}
			// edges
			else {
				if(spl.size() > 0) {
					int p1, p2;
					StringHelper::from_string<int>(spl[0], p1);
					StringHelper::from_string<int>(spl[1], p2);
					particles[p1].add_neigh(&particles[p2]);
				}
			}

			curr_id++;
			curr_line++;
			if(_current_input.eof()) done = true;
		}

		_close_current_input_if_eof();

		// generate the triangles
		vector<RBCTriangle> triangles;
		curr_id = 0;
		for(int i = 0; i < N_mesh; i++) {
			RBCParticle *p = &particles[i];
			for(vector<RBCParticle *>::iterator it = p->neighs.begin(); it != p->neighs.end(); it++) {
				RBCParticle *v1 = *it;
				for(vector<RBCParticle *>::iterator jt = p->neighs.begin(); jt != p->neighs.end(); jt++) {
					RBCParticle *v2 = *jt;
					if(v2->is_neigh_with(v1)) {
						RBCTriangle nt(p->idx, v1->idx, v2->idx);
						bool is_duplicate = false;
						for(vector<RBCTriangle>::iterator tt = triangles.begin(); tt != triangles.end() && !is_duplicate; tt++) if((*tt) == nt) is_duplicate = true;
						if(!is_duplicate) {
							triangles.push_back(nt);
							Triangle *t = new Triangle(p->pos, v1->pos, v2->pos, vec4(1, 0, 0, 1));
							t->set_id(curr_id);
							curr_id++;
							my_scene->add_shape(t);
						}
					}
				}
			}
		}

		vec3 box = 3.f*(max_pos - min_pos);
		my_scene->set_box(box);
	}
	catch (string &error) {
		Logging::log_critical(error);
		exit(1);
	}

	return my_scene;
}
