/*
 * PatchyParser.h
 *
 *  Created on: 26/Sep/2016
 *      Author: Lorenzo
 */

#ifndef PATCHYPARSER_H_
#define PATCHYPARSER_H_

#include <map>
#include <vector>

#include "Parser.h"
#include "IoxDNAParser.h"

class PatchyParser: public Parser, public IoxDNAParser {
protected:
	int _N, _N_A, _N_B;
	int _N_patches_A, _N_patches_B;
	float _delta;
	float _cosmax, _theta;

	std::vector<glm::vec3> _base_patches_A, _base_patches_B;

	std::vector<glm::vec3> _get_base_patches(int);
	void _read_topology(std::string);
public:
	PatchyParser(std::string topology, const std::queue<std::string> &input_files, std::string);
	virtual ~PatchyParser();

	virtual Scene *next_scene();
};

#endif /* PATCHYPARSER_H_ */

