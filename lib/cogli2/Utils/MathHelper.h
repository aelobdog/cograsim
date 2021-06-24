/*
 * MathHelper.h
 *
 *  Created on: 06/nov/2013
 *      Author: lorenzo
 */

#ifndef MATHHELPER_H_
#define MATHHELPER_H_

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace MathHelper {

glm::mat4 get_rotation(glm::vec3 initial, glm::vec3 final);
void orthonormalize(glm::dmat4 &m);
/**
 * Taken from https://github.com/stanluk/SoftBodySim/blob/master/src/sbs/solver/Math.h
 *
 * @param mat
 * @param max_iter
 * @param E
 * @return
 */
glm::vec3 eigenvalues_jacobi(glm::mat3 &mat, glm::mat3 &E);
void diagonalize_3x3(glm::mat3 &mat, glm::vec3 &values, glm::mat3 &vectors);
void eigenvalues_rotate(glm::mat3 &mat, double &c, double &s, int i0, int j0, int i1, int j1);

}

#endif /* MATHHELPER_H_ */
