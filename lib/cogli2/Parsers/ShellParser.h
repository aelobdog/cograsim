/*
 * ShellParser.h
 *
 *  Created on: 1 Mar 2018
 *      Author: lorenzo
 */

#ifndef PARSERS_SHELLPARSER_H_
#define PARSERS_SHELLPARSER_H_

#include "Parser.h"

class ShellParser: public Parser {
public:
	ShellParser(std::string topology, const std::queue<std::string> &input_files);
	virtual ~ShellParser();

	virtual Scene *next_scene();

private:
	std::string _topology_filename;
};

#endif /* PARSERS_SHELLPARSER_H_ */
