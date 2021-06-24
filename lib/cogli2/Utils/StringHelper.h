/*
 * StringHelper.h
 *
 *  Created on: 30/apr/2012
 *      Author: lorenzo
 */

#ifndef STRINGHELPER_H_
#define STRINGHELPER_H_

#include <algorithm>
#include <functional>
#include <locale>
#include <sstream>
#include <vector>
#include <cstdarg>
#include <glm/glm.hpp>

namespace StringHelper {

std::string sformat(const std::string &fmt, ...);
std::string sformat_ap(const std::string &fmt, va_list &ap);

template<class T> bool from_string(const std::string &s, T &t);
template<class T> std::string to_string(const T &t);
std::vector<std::string> split(const std::string &s, char delim = ' ');
std::string join(const std::vector<std::string> &spl, const std::string &glue);
bool get_vec3(const std::vector<std::string> &to_parse, int start, glm::vec3 &dest);

std::string sanitize_filename(std::string);

// trim from start
std::string& ltrim(std::string &s);
// trim from end
std::string& rtrim(std::string &s);
// trim from both ends, it works like Python's own trim
std::string& trim(std::string &s);

std::string& to_upper(std::string &s);
std::string& to_lower(std::string &s);

std::vector<int> ids_from_range(std::string line);

}

#endif /* STRINGHELPER_H_ */
