/*
 * LevyParser.h
 *
 *  Created on: 11/Nov/2013
 *      Author: Flavio
 */

#ifndef LEVYPARSER_H_
#define LEVYPARSER_H_

#include <map>
#include <vector>

#include "Parser.h"
#include "IoxDNAParser.h"

class LevyParser: public Parser, public IoxDNAParser {
protected:
	int _N;
	int _N_tetramers, _N_dimers;

	void _read_topology(std::string);
public:
	LevyParser(std::string topology, const std::queue<std::string> &input_files);
	virtual ~LevyParser();

	virtual Scene *next_scene();
};

#endif /* LEVYPARSER_H_ */

