/*
 * DNARibbon.cpp
 *
 *  Created on: 19/nov/2013
 *      Author: lorenzo
 */

#include <cmath>
#include <fstream>

#include "DNARibbon.h"
#include "../Utils/StringHelper.h"

using glm::vec3;
using glm::vec4;
using std::vector;
using std::endl;
using std::string;

DNARibbon::DNARibbon(DNAStrand &s) : _is_bonded(false) {
	_beg_color = s.nucleotides().begin()->second->color();
	_end_color = _beg_color + vec4(0.3f, 0.3f, 0.3f, 0.f);
	_end_color = glm::clamp(_end_color, 0.f, 1.f);

	_nucleotides = s.nucleotides();
	_N = s.nucleotides().size()-1;

#if LR_TETRA
	std::ifstream in("map.dat");
	if(in.is_open()) {
		int s_id = s.get_id();
		while(in.good()) {
			int p, q;
			in >> p >> q;
			if(p == s_id || q == s_id) _is_bonded = true;
		}
	}
	in.close();
#endif
}

DNARibbon::~DNARibbon() {

}

void DNARibbon::_append_points(std::vector<vec3> &to_app, bspline *spline[], float x) {
	vec3 v1, v2, v3, v4;
	for(int i = 0; i < 3; i++) {
		bspline *bs = spline[i];
		float y = bs->evaluate(x);
		float y1 = bs->evaluate(x+1.f);
		float bs_der_x = (x > 0.f) ? (y1 - bs->evaluate(x-1)) / 2 : y1 - y;
		float bs_der_x1 = (bs->evaluate(x+2) - y) / 2;

		v1[i] = y;
		v2[i] = y + 0.25f*bs_der_x;
		v3[i] = y1 - 0.25f*bs_der_x1;
		v4[i] = y1;
	}

	to_app.push_back(v1);
	to_app.push_back(v2);
	to_app.push_back(v3);
	to_app.push_back(v4);
}

std::string DNARibbon::get_output() {
	// colors
	vector<vec4> colors;
	vector<float> x_axis;
	for(unsigned int i = 0; i < _nucleotides.size(); i++) {
		vec4 nc = _end_color * (i / float(_N)) + _beg_color * (1.f - i/float(_N));
		nc = glm::clamp(nc, 0.f, 1.f);

#ifdef LR_TETRA
		if(i+6 >= _nucleotides.size()) {
			if(_is_bonded) nc = glm::vec4(0.15, 0.6f, 0.15f, 1.f);
//			else nc = glm::vec4(1.f, 0.4f, 0.f, 1.f);
			else nc = glm::vec4(0.8f, 0.28f, 1.f, 1.f);
		}
		else nc = glm::vec4(0.8f, 0.8f, 0.8f, 1.f);
//		nc = glm::vec4(0.8f, 0.8f, 0.8f, 1.f);
#endif

		colors.push_back(nc);
		x_axis.push_back((float) i);
	}

	vector<float> upline[3], umline[3], dmline[3], doline[3];
	for(std::map<int, Nucleotide *>::iterator it = _nucleotides.begin(); it != _nucleotides.end(); it++) {
		vec3 pos = it->second->position() + it->second->backbone();
		vec3 axis = glm::cross(it->second->stacking_axis(), it->second->principal_axis());
		for(int j = 0; j < 3; j++) {
			float width = 0.35f;
			float height = 0.09f;
			upline[j].push_back(pos[j] + axis[j] * width);
			umline[j].push_back(pos[j] + axis[j] * height);
			dmline[j].push_back(pos[j] - axis[j] * height);
			doline[j].push_back(pos[j] - axis[j] * width);
		}
	}

	// See http://www.eol.ucar.edu/homes/granger/bspline/doc/classBSplineBase.html#_details
	int bc = BSplineBase<float>::BC_ZERO_SECOND;
	float wl = 0.f;

	BSplineBase<float> bs_base(&x_axis[0], x_axis.size(), wl, bc);

	bspline *ufx = bs_base.apply(&upline[0][0]);
	bspline *ufy = bs_base.apply(&upline[1][0]);
	bspline *ufz = bs_base.apply(&upline[2][0]);
	bspline *uf[3] = {ufx, ufy, ufz};

	bspline *umx = bs_base.apply(&umline[0][0]);
	bspline *umy = bs_base.apply(&umline[1][0]);
	bspline *umz = bs_base.apply(&umline[2][0]);
	bspline *um[3] = {umx, umy, umz};

	bspline *dmx = bs_base.apply(&dmline[0][0]);
	bspline *dmy = bs_base.apply(&dmline[1][0]);
	bspline *dmz = bs_base.apply(&dmline[2][0]);
	bspline *dm[3] = {dmx, dmy, dmz};

	bspline *dfx = bs_base.apply(&doline[0][0]);
	bspline *dfy = bs_base.apply(&doline[1][0]);
	bspline *dfz = bs_base.apply(&doline[2][0]);
	bspline *df[3] = {dfx, dfy, dfz};

	std::stringstream out;

	std::string patch_head = "bicubic_patch\n{\ntype 0\nu_steps 3\nv_steps 3\nflatness 0\nuv_vectors\n<0,0> <1,0> <1,1> <0,1>\n";
	std::string patch_tail_format = "uv_mapping\ntexture { pigment { gradient u color_map { [0 color rgbt <%g,%g,%g,%g>] "
			"[1 color rgbt <%g,%g,%g,%g>] }} finish {ambient 1 phong 0.6 phong_size 20}} texture { pigment { gradient v "
			"color_map { [0 color rgbt <0,0,0,0.3>] [0.15 color rgbt <0,0,0,1>] [0.85 color rgbt <0,0,0,1>] [1 color "
			"rgbt <0,0,0,0.3>]} }}";

	for(int i = 0; i < _N; i++) {
		std::vector<vec3> points;
		_append_points(points, uf, (float) i);
		_append_points(points, um, (float) i);
		_append_points(points, dm, (float) i);
		_append_points(points, df, (float) i);

		for(int j = 1; j < 5; j++) {
			for(int k = 1; k < 5; k++) {
				int ind = (j-1)*4 + (k-1);
				out << StringHelper::sformat("#local B%d%d = <%g, %g, %g>;\n", j, k, points[ind][0], points[ind][1], points[ind][2]);
			}
		}

		out << patch_head;
		out << "B11, B12, B13, B14\nB21, B22, B23, B24\nB31, B32, B33, B34\nB41, B42, B43, B44\n";

		out << StringHelper::sformat(patch_tail_format.c_str(), colors[i][0], colors[i][1], colors[i][2], 1.f-colors[i][3], colors[i+1][0], colors[i+1][1], colors[i+1][2], 1.f-colors[i+1][3]);
		if(i == 0) {
			// black border for the first one
			out << " texture { pigment { gradient u color_map { [0. color rgbt <0,0,0,0.3>] [0.15 color rgbt <0,0,0,1>] [1 color rgbt <0,0,0,1>]}}}";
		}
		else if(i == (_N-1)) {
			// and for the last one
			out << " texture { pigment { gradient u color_map { [0. color rgbt <0,0,0,1>] [0.85 color rgbt <0,0,0,1>] [1 color rgbt <0,0,0,0.3>]}}}";
		}

		out << " \n}\n\n";
	}

	for(int i = 0; i < 3; i++) {
		delete uf[i];
		delete um[i];
		delete dm[i];
		delete df[i];
	}

	return out.str();
}
