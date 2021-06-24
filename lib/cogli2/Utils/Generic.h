/*
 * Generic.h
 *
 *  Created on: Apr 18, 2020
 *      Author: lorenzo
 */

#ifndef UTILS_GENERIC_H_
#define UTILS_GENERIC_H_

#include <vector>
#include <algorithm>
#include <numeric>
#include <iostream>

#include <glm/gtx/string_cast.hpp>

namespace Generic {

// taken from https://stackoverflow.com/a/17074810/5140209

template<typename T, typename Compare>
std::vector<std::size_t> sort_permutation(const std::vector<T> &vec, const Compare &compare) {
	std::vector<std::size_t> perm(vec.size());
	std::iota(perm.begin(), perm.end(), 0);
	std::sort(perm.begin(), perm.end(), [&](std::size_t i, std::size_t j) {
		return compare(vec[i], vec[j]);
	});
	return perm;
}

template<typename T>
std::vector<T> apply_permutation(const std::vector<T> &vec, const std::vector<std::size_t> &p) {
	std::vector<T> sorted_vec(vec.size());
	std::transform(p.begin(), p.end(), sorted_vec.begin(), [&](std::size_t i) {
		return vec[i];
	});
	return sorted_vec;
}

template<typename T>
void apply_permutation_in_place(std::vector<T> &vec, const std::vector<std::size_t> &p) {
	std::vector<bool> done(vec.size());
	for(std::size_t i = 0; i < vec.size(); ++i) {
		if(done[i]) {
			continue;
		}
		done[i] = true;
		std::size_t prev_j = i;
		std::size_t j = p[i];
		while(i != j) {
			std::swap(vec[prev_j], vec[j]);
			done[j] = true;
			prev_j = j;
			j = p[j];
		}
	}
}

}
;

#endif /* UTILS_GENERIC_H_ */
