/*
 * DNARibbon.h
 *
 *  Created on: 19/nov/2013
 *      Author: lorenzo
 */

#ifndef DNARIBBON_H_
#define DNARIBBON_H_

#include <sstream>
#include <vector>

#include "../Utils/BSpline/BSpline.h"

#include "../Drawables/DNA.h"
#include <glm/glm.hpp>

typedef BSpline<float> bspline;

class DNARibbon {
protected:
	glm::vec4 _beg_color;
	glm::vec4 _end_color;

	std::map<int, Nucleotide *> _nucleotides;
	int _N;
	/// This is used only if LR_TETRA has been defined
	bool _is_bonded;

	void _append_points(std::vector<glm::vec3> &to_app, bspline *spline[], float x);
public:
	DNARibbon(DNAStrand &s);
	virtual ~DNARibbon();

	std::string get_output();
};

#endif /* DNARIBBON_H_ */
