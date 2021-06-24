/*
 * PovrayDrawer.h
 *
 *  Created on: 20/ott/2013
 *      Author: lorenzo
 */

#ifndef POVRAYDRAWER_H_
#define POVRAYDRAWER_H_

#include <cstdlib>

#include "Drawer.h"

class PovDrawer: public Drawer {
protected:
	FILE *_out;
	bool _dna_as_ribbon;
	bool _patchy_as_kf;
	bool _patch_header_printed;
	bool _draw_outlines;
	bool _cast_shadows;

	float _outline_relative_r;
	float _outline_fade_relative_r;
	float _outline_fade_mult;

	std::string _patches_as_textures(PatchyParticle &, const std::string &);
	std::string _patches_as_kf(PatchyParticle &, const std::string &);

public:
	PovDrawer();
	virtual ~PovDrawer();

	void init();
	void set_out_file(FILE *out) { _out = out; }

	void set_DNA_as_ribbon(bool v) { _dna_as_ribbon = v; }
	void set_patchy_as_kf(bool v) { _patchy_as_kf = v; }
	void draw_outlines(bool v) { _draw_outlines = v; }
	void cast_shadows(bool v) { _cast_shadows = v; }

	virtual void visit(Drawable &);
	virtual void visit(Scene &);
	virtual void visit(Cylinder &);
	virtual void visit(Icosahedron &);
	virtual void visit(Sphere &);
	virtual void visit(DNAStrand &);
	virtual void visit(Nucleotide &);
	virtual void visit(DHS &);
	virtual void visit(PatchyParticle &);
	virtual void visit(Arrow &);
	virtual void visit(Triangle &);
	virtual void visit(Group &);
};

#endif /* POVRAYDRAWER_H_ */
