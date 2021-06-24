/*
 * StringHelper.cpp
 *
 *  Created on: 30/apr/2012
 *      Author: lorenzo
 */

#include "StringHelper.h"
#include "Logging.h"

#include <algorithm>
#include <iostream>

namespace StringHelper {

std::string sformat(const std::string &fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	std::string str = sformat_ap(fmt, ap);
	va_end(ap);
	return str;
}

// c++ wrapper around sprintf (WTF?)
std::string sformat_ap(const std::string &fmt, va_list &ap) {
	int size = 500;
	std::string str;
	while(1) {
		str.resize(size);
		int n = vsnprintf((char *) str.c_str(), size, fmt.c_str(), ap);
		if(n > -1 && n < size) {
			str.resize(n);
			return str;
		}
		if(n > -1) size = n + 1;
		else size *= 2;
	}
	return str;
}

bool get_vec3(const std::vector<std::string> &to_parse, int start, glm::vec3 &dest) {
	if(to_parse.size() < 3) return false;
	if(!from_string<float>(to_parse[start], dest[0])) return false;
	if(!from_string<float>(to_parse[start + 1], dest[1])) return false;
	if(!from_string<float>(to_parse[start + 2], dest[2])) return false;

	return true;
}

template<>
bool from_string(const std::string &s, bool &t) {
	std::string ns(s);
	std::transform(ns.begin(), ns.end(), ns.begin(), ::tolower);
	if(ns == "0" || ns == "false") {
		t = false;
		return true;
	}

	if(ns == "1" || ns == "true") {
		t = true;
		return true;
	}

	return false;
}

template<class T>
bool from_string(const std::string &s, T &t) {
	std::istringstream iss(s);
	return !(iss >> t).fail();
}

template<class T>
std::string to_string(const T &t) {
	std::stringstream iss;
	iss << t;

	return iss.str();
}

std::vector<std::string> split(const std::string &s, char delim) {
	std::string s_copy(s);
	if(delim == ' ') trim(s_copy);
	std::vector<std::string> elems;
	std::stringstream ss(s_copy);
	std::string item;

	while(getline(ss, item, delim)) {
		if(delim == ' ') {
			trim(item);
			if(item.length() > 0) elems.push_back(item);
		}
		else elems.push_back(item);
	}

	return elems;
}

std::string join(const std::vector<std::string> &spl, const std::string &glue) {
	std::string res = spl[0];

	std::vector<std::string>::const_iterator it = spl.begin();
	for(++it; it != spl.end(); it++) {
		res += glue;
		res += *it;
	}

	return res;
}

std::string sanitize_filename(std::string filename) {
	filename.erase(std::remove(filename.begin(), filename.end(), ':'), filename.end());
	filename.erase(std::remove(filename.begin(), filename.end(), ','), filename.end());
	std::replace(filename.begin(), filename.end(), ' ', '_');
	return filename;
}

std::string &ltrim(std::string &s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
	return s;
}

std::string &rtrim(std::string &s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
	return s;
}

std::string &trim(std::string &s) {
	return ltrim(rtrim(s));
}

std::string &to_upper(std::string &s) {
	std::transform(s.begin(), s.end(), s.begin(), ::toupper);
	return s;
}

std::string &to_lower(std::string &s) {
	std::transform(s.begin(), s.end(), s.begin(), ::tolower);
	return s;
}

std::vector<int> ids_from_range(std::string particle_string) {
	// first remove all the spaces from the string, so that the parsing goes well.
	particle_string.erase(std::remove(particle_string.begin(), particle_string.end(), ' '), particle_string.end());

	std::vector<std::string> temp = split(particle_string.c_str(), ',');
	std::vector<int> particles_index;

	for(std::vector<std::string>::size_type i = 0; i < temp.size(); i++) {
		bool found_dash = temp[i].find('-') != std::string::npos;
		// if the string contains a dash, then it has to be interpreted as a list of particles
		// unless it's a negative number

		if(found_dash && '-' != temp[i].c_str()[0]) {
			// get the two indices p0 and p1 and check they make sense
			std::vector<std::string> p0_p1_index = split(temp[i].c_str(), '-');

			int p[2] = { 0 };
			// check whether the p0 and p1 keys can be understood, and set them
			for(int ii = 0; ii < 2; ii++) {
				if(!from_string<int>(p0_p1_index[ii], p[ii])) {
					Logging::log_critical("Could not interpret particle identifier %s used as a boundary particle in line '%s'\n", p0_p1_index[ii].c_str(), particle_string.c_str());
					exit(1);
				}
			}

			// add all the particles between p0 and p1 (extremes included)
			if(p[0] >= p[1]) {
				Logging::log_critical("The two indexes in a particle range (here %d and %d) should be sorted (the first one should be smaller than the second one).", p[0], p[1]);
				exit(1);
			}
			for(int p_idx = p[0]; p_idx <= p[1]; p_idx++) {
				particles_index.push_back(p_idx);
			}

		}
		// add it to the vector, and make sure that the identifier is not an unidentified string
		else {
			int j;
			if(!from_string<int>(temp[i], j)) {
				Logging::log_critical("Could not interpret particle identifier %s in line %s", temp[i].c_str(), particle_string.c_str());
				exit(1);

			}
			particles_index.push_back(j);
		}

	}

	// check that no particle appears twice
	for(std::vector<int>::size_type i = 0; i < particles_index.size(); i++) {
		for(std::vector<int>::size_type j = i + 1; j < particles_index.size(); j++) {
			if(particles_index[i] == particles_index[j]) {
				Logging::log_critical("Particle index %d appears twice (both at position %d and at position %d), but each index can only appear once. Dying badly.", particles_index[i], i + 1, j + 1);
				exit(1);
			}
		}
	}
	// finally return the vector.
	return particles_index;
}

template bool from_string<float>(const std::string &s, float &t);
template bool from_string<unsigned long int>(const std::string &s, unsigned long int &t);
template bool from_string<unsigned int>(const std::string &s, unsigned int &t);
template bool from_string<double>(const std::string &s, double &t);
template bool from_string<int>(const std::string &s, int &t);
template bool from_string<long long int>(const std::string &s, long long int &t);
template bool from_string<bool>(const std::string &s, bool &t);
template std::string to_string<double>(const double &t);
template std::string to_string<int>(const int &t);

}
