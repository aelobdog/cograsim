/*
 * ActionBuilder.h
 *
 *  Created on: 31 gen 2018
 *      Author: lorenzo
 */

#ifndef ACTIONS_ACTIONBUILDER_H_
#define ACTIONS_ACTIONBUILDER_H_

#include <vector>
#include <string>
#include <memory>

class BaseAction;
class Camera;

class ActionBuilder {
private:
	ActionBuilder();
public:
	virtual ~ActionBuilder();

	static std::vector<std::shared_ptr<BaseAction>> actions_from_file(std::string filename, Camera &camera);
};

#endif /* ACTIONS_ACTIONBUILDER_H_ */
