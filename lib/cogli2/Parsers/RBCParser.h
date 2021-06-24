/*
 * RBCParser.h
 *
 *  Created on: 20/ott/2013
 *      Author: lorenzo
 */

#ifndef RBCPARSER_H_
#define RBCPARSER_H_

#include "Parser.h"
#include <glm/glm.hpp>

class RBCParser: public Parser {
public:
	RBCParser(const std::queue<std::string> &input_files);
	virtual ~RBCParser();

	virtual Scene *next_scene();
};

class RBCParticle {
public:
	int idx;
	glm::vec3 pos;
	std::vector<RBCParticle *> neighs;

	RBCParticle() : idx(-1) { };

	void set_index(int nidx) {
		idx = nidx;
	}

	bool is_neigh_with(RBCParticle *p) {
		for(std::vector<RBCParticle *>::iterator it = neighs.begin(); it != neighs.end(); it++) {
			if(p == *it) return true;
		}
		return false;
	}

	void set_pos(glm::vec3 np) {
		pos = np;
	}

	void add_neigh(RBCParticle *p) {
		if(!is_neigh_with(p)) {
			neighs.push_back(p);
			p->neighs.push_back(this);
		}
	}
};

class RBCTriangle {
public:
	int p1, p2, p3;

	RBCTriangle(int np1, int np2, int np3) {
		if(np1 < np2 && np1 < np3) {
			p1 = np1;
			if(np2 < np3) {
				p2 = np2;
				p3 = np3;
			}
			else {
				p2 = np3;
				p3 = np2;
			}
		}

		if(np2 < np1 && np2 < np3) {
			p1 = np2;
			if(np1 < np3) {
				p2 = np1;
				p3 = np3;
			}
			else {
				p2 = np3;
				p3 = np1;
			}
		}

		if(np3 < np2 && np3 < np1) {
			p1 = np3;
			if(np1 < np2) {
				p2 = np1;
				p3 = np2;
			}
			else {
				p2 = np2;
				p3 = np1;
			}
		}
	}

	bool operator==(const RBCTriangle &other) const {
		return (p1 == other.p1 && p2 == other.p2 && p3 == other.p3);
	}
};

#endif /* RBCPARSER_H_ */
