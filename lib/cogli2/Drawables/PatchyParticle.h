/*
 * PatchyParticle.h
 *
 *  Created on: 17/dic/2013
 *      Author: lorenzo
 */

#ifndef PATCHYPARTICLE_H_
#define PATCHYPARTICLE_H_

#include <vector>

#include "Sphere.h"

struct Patch {
	glm::vec3 pos;
	float width;
	float cosmax;
	glm::vec4 color;

	Patch(glm::vec3 n_pos, float n_width, glm::vec4 n_color);
	virtual ~Patch();
};

class PatchyParticle: public Sphere {
protected:
	std::vector<Patch> _patches;
public:
	PatchyParticle(glm::vec3 pos, float r, glm::vec4 color);
	virtual ~PatchyParticle();

	void add_patch(glm::vec3 pos, float width, glm::vec4 color);
	std::vector<Patch> get_patches() { return _patches; }
	virtual void rotate(glm::mat3, glm::vec3);

	void accept_drawer(Drawer &d);
};

#endif /* PATCHYPARTICLE_H_ */
