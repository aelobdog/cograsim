/*
 * StarrParser.h
 *
 *  Created on: 11/Nov/2013
 *      Author: Flavio
 */

#ifndef STARRPARSER_H_
#define STARRPARSER_H_

#include <map>
#include <vector>

#include "Parser.h"
#include "IoxDNAParser.h"

class StarrParser: public Parser, public IoxDNAParser {
protected:
	int _N;
	int _mode;
	std::vector<std::string> _seqs;
	int _N_tetramers, _N_dimers, _N_strands;
	int _N_dimer_spacers;

	void _read_topology(std::string);
public:
	enum {
		STRANDS = 0,
		TETRAMERS = 1,
		VITRIMERS = 2
	};

	StarrParser(std::string topology, const std::queue<std::string> &input_files, const char *opts);
	virtual ~StarrParser();

	virtual Scene *next_scene();
};

#endif /* STARRPARSER_H_ */

