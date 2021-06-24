/*
 * MGLParser.h
 *
 *  Created on: 20/ott/2013
 *      Author: lorenzo
 */

#ifndef MGLPARSER_H_
#define MGLPARSER_H_

#include "Parser.h"
#include <glm/glm.hpp>

class MGLParser: public Parser {
protected:
	glm::vec3 _get_box(std::string &line);

	Shape *_build_shape(std::vector<std::string> &spl_line);
public:
	MGLParser(const std::queue<std::string> &input_files);
	virtual ~MGLParser();

	virtual Scene *next_scene();
};

#endif /* MGLPARSER_H_ */
