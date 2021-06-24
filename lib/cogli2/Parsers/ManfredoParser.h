/*
 * ManfredoParser.h
 *
 *  Created on: 11/Nov/2013
 *      Author: Flavio
 */

#ifndef MANFREDOPARSER_H_
#define MANFREDOPARSER_H_

#include <map>
#include <vector>

#include "Parser.h"
#include "IoxDNAParser.h"

class ManfredoParser: public Parser, public IoxDNAParser {
protected:
	int _N;
	int _N_tetra;
	int _N_per_tetra;
	void _read_topology(std::string);
public:
	ManfredoParser(std::string topology, const std::queue<std::string> &input_files);
	virtual ~ManfredoParser();

	virtual Scene *next_scene();
};

#endif /* MANFREDOPARSER_H_ */

