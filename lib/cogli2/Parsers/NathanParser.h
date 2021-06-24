/*
 * NathanParser.h
 *
 *  Created on: 11/Nov/2013
 *      Author: Lorenzo
 */

#ifndef NATHANPARSER_H_
#define NATHANPARSER_H_

#include <map>
#include <vector>

#include "Parser.h"
#include "IoxDNAParser.h"

#define NATHAN_PATCH_WIDTH 0.6919551751263169f
#define NATHAN_PATCH_DELTA 0.09f

class NathanParser: public Parser, public IoxDNAParser {
protected:
	float _pol_r;
	int _N;
	int _N_patchy;
	int _N_per_chain;
	int _N_chains;
	void _read_topology(std::string);
public:
	NathanParser(std::string topology, const std::queue<std::string> &input_files, float size_ratio);
	virtual ~NathanParser();

	virtual Scene *next_scene();
};

#endif /* NATHANPARSER_H_ */

