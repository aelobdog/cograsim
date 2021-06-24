/*
 * PovrayDrawer.cpp
 *
 *  Created on: 20/ott/2013
 *      Author: lorenzo
 */

#include <vector>
#include <sstream>
#include <string>
#include <iostream>

#include "PovDrawer.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include "../../Utils/MathHelper.h"
#include "../../Utils/StringHelper.h"
#include "../povray/DNARibbon.h"

using namespace std;
using glm::vec3;
using glm::vec4;
using glm::mat4;

PovDrawer::PovDrawer() :
		_out(NULL), _dna_as_ribbon(false), _patchy_as_kf(false), _patch_header_printed(false), _draw_outlines(false), _cast_shadows(false) {
	_outline_relative_r = 0.04;
	_outline_fade_relative_r = 0.002;
	_outline_fade_mult = 5.f;
}

PovDrawer::~PovDrawer() {

}

void PovDrawer::init() {
	_patch_header_printed = false;
}

void PovDrawer::visit(Drawable &d) {

}

void PovDrawer::visit(Scene &s) {
	for(auto shape : s.get_shapes()) {
		if(shape->is_visible()) {
			shape->accept_drawer(*this);
		}
	}

	if(s.is_box_visible()) {
		fprintf(_out, "// box output\n");
		for(auto cyl : s.get_box()) {
			cyl->accept_drawer(*this);
		}
		fprintf(_out, "// end of box output\n");
	}
}

void PovDrawer::visit(Cylinder &c) {
	vec4 color = c.color();
	vec3 base = c.position();
	vec3 cap = base + c.axis();

	float r = c.r();
	if(_draw_outlines) {
		r -= _outline_relative_r * r;
		base += _outline_relative_r * c.axis();
		cap -= _outline_relative_r * c.axis();
	}

	// some cylinders are not strictly particles, so they don't have an associated id. If this is the case then
	// we don't print the first output line
	if(c.get_id() != -1) fprintf(_out, "\n// particle %d\n", c.get_id());
	fprintf(_out, "cylinder {\n");
	fprintf(_out, "\t<%f, %f, %f>,  <%f, %f, %f>, %f\n", base[0], base[1], base[2], cap[0], cap[1], cap[2], r);
	fprintf(_out, "\tpigment { color rgbt <%g,%g,%g,%g> }\n", color[0], color[1], color[2], 1.f - color[3]);
	if(!_cast_shadows) fprintf(_out, "\tno_shadow\n");
	fprintf(_out, "}\n");

	if(_draw_outlines) {
		float r_outline = c.r() + _outline_fade_relative_r;
		float fade_distance = (r_outline - r) * _outline_fade_mult;
		base -= _outline_relative_r * c.axis();
		cap += _outline_relative_r * c.axis();
		fprintf(_out, "cylinder {\n");
		fprintf(_out, "\t<%f, %f, %f>,  <%f, %f, %f>, %f\n", base[0], base[1], base[2], cap[0], cap[1], cap[2], r_outline);
		fprintf(_out, "\tpigment { color rgbt 1 }\n");
		fprintf(_out, "\tinterior {\n");
		fprintf(_out, "\t\tfade_distance %f\n\t\tfade_color rgb 0\n\t\tfade_power 999\n", fade_distance);
		fprintf(_out, "\t}\n");
		fprintf(_out, "\tno_shadow\n");
		fprintf(_out, "\thollow\n}\n");
	}
}

void PovDrawer::visit(Icosahedron &s) {
	printf ("povray for icosahedra not implemented yet\n");
	return;
}

void PovDrawer::visit(Sphere &s) {
	vec4 color = s.color();
	vec3 pos = s.position();
	float r = s.r();
	if(_draw_outlines) {
		r -= _outline_relative_r * r;
	}

	fprintf(_out, "\n// particle %d\n", s.get_id());
	fprintf(_out, "sphere {\n");
	fprintf(_out, "\t<%f, %f, %f> %f\n", pos[0], pos[1], pos[2], r);
	fprintf(_out, "\tpigment { color rgbt <%g,%g,%g,%g> }\n", color[0], color[1], color[2], 1.f - color[3]);
	if(!_cast_shadows) fprintf(_out, "\tno_shadow\n");
	fprintf(_out, "}\n");

	if(_draw_outlines) {
		float r_outline = s.r() + _outline_fade_relative_r;
		float fade_distance = (r_outline - r) * _outline_fade_mult;
		fprintf(_out, "\n// outline of particle %d\n", s.get_id());
		fprintf(_out, "sphere {\n");
		fprintf(_out, "\t<%f, %f, %f> %f\n", pos[0], pos[1], pos[2], r_outline);
		fprintf(_out, "\tpigment { color rgbt 1 }\n");
		fprintf(_out, "\tinterior {\n");
		fprintf(_out, "\t\tfade_distance %f\n\t\tfade_color rgb 0\n\t\tfade_power 999\n", fade_distance);
		fprintf(_out, "\t}\n");
		fprintf(_out, "\tno_shadow\n");
		fprintf(_out, "\thollow\n}\n");
	}
}

void PovDrawer::visit(Group &) {

}

void PovDrawer::visit(DNAStrand &strand) {
	fprintf(_out, "// strand %d\n", strand.get_id());
	if(_dna_as_ribbon) {
		DNARibbon ribbon(strand);
		fprintf(_out, "%s\n", ribbon.get_output().c_str());
	}
	else {
		std::map<int, Nucleotide *> nucleotides = strand.nucleotides();
		std::map<int, Nucleotide *>::iterator it = nucleotides.begin();

		Nucleotide *last = NULL;
		for(; it != nucleotides.end(); it++) {
			Nucleotide *current = it->second;
			// we have to design the backbone-backbone links
			if(it != nucleotides.begin()) {
				vec3 curr_back_pos = current->position() + current->backbone();
				vec3 last_back_pos = last->position() + last->backbone();

				vec3 cyl_axis = last_back_pos - curr_back_pos;

				Cylinder cyl_shape(curr_back_pos, cyl_axis, 0.13f, current->color());
				cyl_shape.accept_drawer(*this);
			}
			it->second->accept_drawer(*this);
			last = it->second;
		}

		// if the strand is circular we also add a cylinder between the first and the last nucleotides
		if(strand.is_circular()) {
			Nucleotide *first = nucleotides.begin()->second;
			vec3 curr_back_pos = first->position() + first->backbone();
			vec3 last_back_pos = last->position() + last->backbone();

			vec3 cyl_axis = last_back_pos - curr_back_pos;

			Cylinder cyl_shape(curr_back_pos, cyl_axis, 0.13f, first->color());
			cyl_shape.accept_drawer(*this);
		}
	}
}

void PovDrawer::visit(Nucleotide &n) {
	vec3 x = n.principal_axis();
	vec3 z = n.stacking_axis();
	vec3 pos = n.position();

	fprintf(_out, "\n// nucleotide %d\n", n.get_id());

	// backbone
	vec3 back_pos = pos + n.backbone();
	float back_r = 0.25f;

	Sphere back_sphere(back_pos, back_r, n.color());
	back_sphere.accept_drawer(*this);

	// base
	vec3 base_pos = pos + x * 0.3f;
	float base_r = 0.1f;
	vec3 z_lab(0.f, 0.f, 1.f);
	mat4 x_rot = MathHelper::get_rotation(z_lab, x);
	vec3 x_lab(1.f, 0.f, 0.f);
	vec3 new_x = glm::mat3(x_rot) * x_lab;
	mat4 z_rot = MathHelper::get_rotation(new_x, z);
	glm::mat3 tot_rot(z_rot * x_rot);
	vec4 base_color = n.base_color();

	// cylinder com --> base
	float cyl_r = 0.1f;
	Cylinder cyl_com_base(pos, x * 0.3f * 0.8f, cyl_r, n.color());
	cyl_com_base.accept_drawer(*this);

	// cylinder backbone -- com
	Cylinder cyl_back_base(pos, n.backbone(), cyl_r, n.color());
	cyl_back_base.accept_drawer(*this);

	// sphere at com
	Sphere base_sphere(pos, cyl_r, n.color());
	base_sphere.accept_drawer(*this);

	float fade_distance = 0.;
	if(_draw_outlines) fade_distance = 0.06;

	fprintf(_out, "sphere {\n");
	fprintf(_out, "\t0, %g\n\tscale<%f,%f,%f>\n", 1. - 1.25 * fade_distance, 0.8f * base_r, 2.f * base_r, 3.f * base_r);
	fprintf(_out, "\tmatrix <%f, %f, %f,\n", tot_rot[0][0], tot_rot[0][1], tot_rot[0][2]);
	fprintf(_out, "\t\t%f, %f, %f,\n", tot_rot[1][0], tot_rot[1][1], tot_rot[1][2]);
	fprintf(_out, "\t\t%f, %f, %f,\n", tot_rot[2][0], tot_rot[2][1], tot_rot[2][2]);
	fprintf(_out, "\t\t%f, %f, %f>\n", base_pos[0], base_pos[1], base_pos[2]);
	fprintf(_out, "\tpigment { color rgbt <%g,%g,%g,%g> }\n", base_color[0], base_color[1], base_color[2], 1.f - base_color[3]);
	if(!_cast_shadows) fprintf(_out, "\tno_shadow\n");
	fprintf(_out, "}\n");

	if(_draw_outlines) {
		fprintf(_out, "sphere {\n");
		fprintf(_out, "\t0, 1\n\tscale<%f,%f,%f>", 0.8f * base_r, 2.f * base_r, 3.f * base_r);
		fprintf(_out, "\tpigment { color rgbt <1, 1, 1, 1> }\n");
		fprintf(_out, "\tinterior {\n");
		fprintf(_out, "\t\tfade_distance %f\n\t\tfade_color rgb 0\n\t\tfade_power 999\n", fade_distance);
		fprintf(_out, "\t}\n");
		fprintf(_out, "\tmatrix <%f, %f, %f,\n", tot_rot[0][0], tot_rot[0][1], tot_rot[0][2]);
		fprintf(_out, "\t\t%f, %f, %f,\n", tot_rot[1][0], tot_rot[1][1], tot_rot[1][2]);
		fprintf(_out, "\t\t%f, %f, %f,\n", tot_rot[2][0], tot_rot[2][1], tot_rot[2][2]);
		fprintf(_out, "\t\t%f, %f, %f>\n", base_pos[0], base_pos[1], base_pos[2]);
		//fprintf(_out, "\thollow\n");
		fprintf(_out, "\tno_shadow\n}\n");
	}
}

void PovDrawer::visit(DHS &s) {
	vec3 dipole = s.dipole();
	vec3 position = s.position();

	Sphere sphere(s);
	sphere.accept_drawer(*this);

	// the arrow is always black
	vec4 color = glm::vec4(0.f, 0.f, 0.f, 1.f);

	float arrow_length = DIP_FACTOR * glm::length(dipole);
	float cyl_r = 2 * s.r() * DIP_THICKNESS * 0.4f;
	vec3 cyl_position = position - dipole * (0.5f * arrow_length);
	vec3 cyl_axis = dipole * DIP_FACTOR * (1.f - DIP_ARROW_RATIO);

	Cylinder cyl(cyl_position, cyl_axis, cyl_r, color);
	cyl.accept_drawer(*this);

	// cone
	float cone_r = 2 * s.r() * DIP_THICKNESS * 0.4f * 2.f;
	vec3 cone_base = cyl_position + cyl_axis;
	vec3 cone_apex = cone_base + dipole * DIP_FACTOR * DIP_ARROW_RATIO;
	fprintf(_out, "cone {\n");
	fprintf(_out, "\t<%f, %f, %f>, %f\n", cone_base[0], cone_base[1], cone_base[2], cone_r);
	fprintf(_out, "\t<%f, %f, %f>, 0\n", cone_apex[0], cone_apex[1], cone_apex[2]);
	fprintf(_out, "\tpigment { color rgbt <%g,%g,%g,%g> }\n", color[0], color[1], color[2], 1.f - color[3]);
	if(!_cast_shadows) fprintf(_out, "\tno_shadow\n");
	fprintf(_out, "}\n");
}

void PovDrawer::visit(Arrow &a) {
	vec3 axis = a.axis();
	vec3 position = a.position();

	// the arrow is always black
	vec4 color = glm::vec4(0.f, 0.f, 0.f, 1.f);

	float arrow_length = glm::length(axis);
	float cyl_r = 0.4 * a.r_cyl();
	vec3 cyl_position = position;
	vec3 cyl_axis = axis * a.cyl_length() / arrow_length;

	Cylinder cyl(cyl_position, cyl_axis, cyl_r, color);
	cyl.accept_drawer(*this);

	// cone
	float cone_r = a.r_cyl();
	vec3 cone_base = cyl_position + cyl_axis;
	vec3 cone_apex = axis;
	fprintf(_out, "cone {\n");
	fprintf(_out, "\t<%f, %f, %f>, %f\n", cone_base[0], cone_base[1], cone_base[2], cone_r);
	fprintf(_out, "\t<%f, %f, %f>, 0\n", cone_apex[0], cone_apex[1], cone_apex[2]);
	fprintf(_out, "\tpigment { color rgbt <%g,%g,%g,%g> }\n", color[0], color[1], color[2], 1.f - color[3]);
	if(!_cast_shadows) fprintf(_out, "\tno_shadow\n");
	fprintf(_out, "}\n");
}

string PovDrawer::_patches_as_textures(PatchyParticle &p, const string &texture_name) {
	stringstream ss;
	// we start by defining some utility functions (dirty way of doing it just once);
	if(!_patch_header_printed) {
		ss << "#declare dot = function (x1, y1, z1, x2, y2, z2) {\n"
				"	x1*x2 + y1*y2 + z1*z2\n"
				"}\n"
				"#declare patch = function(x, y, z, R, cosmax, x0, y0, z0, weight) {\n"
				"	weight*(dot(x/R, y/R, z/R, x0, y0, z0) > cosmax)\n"
				"};\n";
		_patch_header_printed = true;
	}

	vec4 color = p.color();
	vec3 pos = p.position();
	float r = p.r();

	vector<Patch> patches = p.get_patches();
	vector<string> weighted_functions;
	vector<string> textures;
	string texture_template("		[ %g pigment { color rgbt<%g,%g,%g,%g> } normal { ripples 0.25 frequency 20 turbulence 0.2 lambda 5 } finish { phong 0.6 phong_size 40. metallic } ]\n");
	for(unsigned int i = 0; i < patches.size(); i++) {
		Patch &patch = patches[i];
		float weight = (i + 1.) / (float) patches.size();
		// range of the patch (starting from the centre of the particle)
		float rp = glm::length(patch.pos);
		// patch versor
		glm::vec3 vp = patch.pos / rp;
		// patch color
		glm::vec4 cp = patch.color;

		// in this line we call the patch function and pass it the relative position on the surface (x-%f, y-%f, z-%f),
		// the radius of the particle r, and the patch cosmax and versor
		string w_str = StringHelper::sformat("patch(x-%f, y-%f, z-%f, %f, %f, %f, %f, %f, %f)", pos[0], pos[1], pos[2], r, patch.cosmax, vp.x, vp.y, vp.z, weight);
		weighted_functions.push_back(w_str);

		string t_str = StringHelper::sformat(texture_template.c_str(), weight, cp[0], cp[1], cp[2], 1 - cp[3]);
		textures.push_back(t_str);
	}

	string weighted_str = StringHelper::join(weighted_functions, string(" + "));
	string texture_map = StringHelper::join(textures, string("\n"));

	// particle's color in rgbt
	string cp_str = StringHelper::sformat("%g,%g,%g,%g", color[0], color[1], color[2], 1.f - color[3]);

	ss << "#declare " << texture_name << " = texture {\n"
			"	function  { " << weighted_str << " }\n"
			"	texture_map {\n"
			"		[ 0 pigment { color rgbt<" << cp_str << "> } normal { ripples 0.25 frequency 20 turbulence 0.2 lambda 5 } finish { phong 0.3 phong_size 40. } ]\n";
	ss << texture_map << "}\n"
			"}\n";

	return ss.str();
}

string PovDrawer::_patches_as_kf(PatchyParticle &p, const string &texture_name) {
	stringstream ss;

	vec4 color = p.color();
	vec3 pos = p.position();
	float r = p.r();

	string p_pos_str = StringHelper::sformat("<%g,%g,%g>", pos[0], pos[1], pos[2]);

	vector<Patch> patches = p.get_patches();
	for(unsigned int i = 0; i < patches.size(); i++) {
		Patch &patch = patches[i];
		// range of the patch (starting from the centre of the particle)
		float rp = glm::length(patch.pos);
		float cosmax = patch.cosmax;
		float sinmax = sinf(acosf(patch.cosmax));
		// patch versor
		glm::vec3 vp = patch.pos / rp;
		// patch color
		glm::vec4 cp = patch.color;

		// position of the top of the truncated cone and its radius
		glm::vec3 cone_base = pos + vp * r * cosmax;
		float cone_base_r = r * sinmax;
		string base_str = StringHelper::sformat("	<%g,%g,%g> %g\n", cone_base[0], cone_base[1], cone_base[2], cone_base_r);
		// position of the base of the truncated cone and its radius
		glm::vec3 cone_cap = pos + vp * (rp * cosmax);
		float cone_cap_r = rp * sinmax;
		string cap_str = StringHelper::sformat("	<%g,%g,%g> %g\n", cone_cap[0], cone_cap[1], cone_cap[2], cone_cap_r);

		// patch's color in rgbt
		string cp_str = StringHelper::sformat("%g,%g,%g,%g", cp[0], cp[1], cp[2], 1.f - cp[3]);
		// first the cone
		ss << "cone {\n" << base_str << cap_str << "	open\n"
				"	pigment { color rgbt<" << cp_str << "> }\n"
				"	no_shadow\n"
				"}\n";
		// and then the spherical cap

		// we start by finding the plane whose normal is vp. This is defined by vp[0]*x + vp[1]*y + vp[2]*z + d = 0
		// we find d by intersecting cone_cap with this plane
		float d = -(vp[0] * cone_cap[0] + vp[1] * cone_cap[1] + vp[2] * cone_cap[2]);
		string clipping_plane = StringHelper::sformat("<%g,%g,%g>, %g", -vp[0], -vp[1], -vp[2], d);

		ss << "sphere {\n	" << p_pos_str << " " << rp << "\n"
				"	pigment { color rgbt<" << cp_str << "> }\n"
				"	clipped_by { plane { " << clipping_plane << " } }\n"
				"	no_shadow\n"
				"}\n";

	}

	// particle's color in rgbt
	string cp_str = StringHelper::sformat("%g,%g,%g,%g", color[0], color[1], color[2], 1.f - color[3]);

	ss << "#declare " << texture_name << " = texture {\n"
			"		pigment { color rgbt<" << cp_str << "> } normal { ripples 0.25 frequency 20 turbulence 0.2 lambda 5 } finish { phong 0.3 phong_size 40. }\n"
			"}\n";

	return ss.str();
}

void PovDrawer::visit(PatchyParticle &p) {
	vec3 pos = p.position();
	float r = p.r();
	if(_draw_outlines) r -= _outline_relative_r * r;

	string texture_name = StringHelper::sformat("my_texture_%d", p.get_id());
	string patches = (_patchy_as_kf) ? _patches_as_kf(p, texture_name) : _patches_as_textures(p, texture_name);

	fprintf(_out, "%s", patches.c_str());

	fprintf(_out, "\n// patchy particle %d\n", p.get_id());
	fprintf(_out, "sphere {\n");
	fprintf(_out, "\t<%f, %f, %f> %f\n", pos[0], pos[1], pos[2], r);
	fprintf(_out, "\ttexture { %s }\n", texture_name.c_str());
	fprintf(_out, "\tinterior { ior 1. }\n");
	fprintf(_out, "\thollow\n");
	if(!_cast_shadows) fprintf(_out, "\tno_shadow\n");
	fprintf(_out, "}\n");

	if(_draw_outlines) {
		float r_outline = p.r() + _outline_fade_relative_r;
		float fade_distance = (r_outline - r) * _outline_fade_mult;
		fprintf(_out, "\n// outline of particle %d\n", p.get_id());
		fprintf(_out, "sphere {\n");
		fprintf(_out, "\t<%f, %f, %f> %f\n", pos[0], pos[1], pos[2], r_outline);
		fprintf(_out, "\tpigment { color rgbt 1 }\n");
		fprintf(_out, "\tinterior {\n");
		fprintf(_out, "\t\tfade_distance %f\n\t\tfade_color rgb 0\n\t\tfade_power 999\n", fade_distance);
		fprintf(_out, "\t}\n");
		fprintf(_out, "\tno_shadow\n");
		fprintf(_out, "\thollow\n}\n");
	}
}

void PovDrawer::visit(Triangle &t) {
	stringstream ss;
	vector<vec3> vertices = t.vertices();
	vec4 color = t.color();

	ss << "\n// triangle %d" << endl;
	ss << "triangle {" << endl;
	for(int i = 0; i < 3; i++) {
		if(i > 0) ss << ", ";
		else ss << "\t";
		vec3 vert = vertices[i];
		ss << StringHelper::sformat("<%f, %f, %f>", vert[0], vert[1], vert[2]);
	}
	ss << endl;
	ss << StringHelper::sformat("\tpigment { color rgbt <%g,%g,%g,%g> }", color[0], color[1], color[2], 1.f - color[3]) << endl;
	if(!_cast_shadows) ss << "\tno_shadow" << endl;
	ss << "}" << endl;

	fprintf(_out, "%s", ss.str().c_str());
}
