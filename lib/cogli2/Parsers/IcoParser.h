/*
 * IcoParser.h
 *
 *  Created on: 25/Lug/2018
 *      Author: Flavio
 */

#ifndef ICOPARSER_H_
#define ICOPARSER_H_

#include <map>
#include <vector>

#include "Parser.h"
#include "IoxDNAParser.h"

class IcoParser: public Parser, public IoxDNAParser {
protected:
	int _N;
	void _read_topology(std::string);
	std::vector<glm::vec4> _mycolors;
	std::vector<glm::vec3> _ppos;
	std::vector<glm::vec4> _pcol;
public:
	IcoParser(std::string topology, const std::queue<std::string> &input_files);
	virtual ~IcoParser();

	virtual Scene *next_scene();
};

#endif /* ICOPARSER_H_ */

