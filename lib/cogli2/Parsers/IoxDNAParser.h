/*
 * IoxDNAParser.h
 *
 *  Created on: 11/Nov/2013
 *      Author: Flavio
 */

#ifndef IOXDNAPARSER_H_
#define IOXDNAPARSER_H_

#include <map>
#include <vector>
#include <fstream>

#include <glm/glm.hpp>
#include "../Drawables/Scene.h"

class IoxDNAParser {
protected:
	long long int _time;

	virtual void _parse_headers(std::ifstream &_current_input, Scene *my_scene);
	glm::vec3 _get_box(std::string &line);
	virtual void _read_topology(std::string) = 0;
public:
	IoxDNAParser();
	virtual ~IoxDNAParser();
};

#endif /* IOXDNAPARSER_H_ */
