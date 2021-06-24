/*
 * Camera.cpp
 *
 *  Created on: 27/ott/2013
 *      Author: lorenzo
 */

#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include "Utils/Logging.h"
#include "Utils/StringHelper.h"

#include <cmath>
#include <iostream>
#include <sstream>
#include <fstream>

using namespace glm;
using std::endl;

Camera::Camera() {
	_is_ortho = _saved_is_ortho = false;
	_fov = _saved_fov = 45.0f;
	_left_button = false;
	_right_button = false;
	_ref_cursor_pos_x = 0.f;
	_ref_cursor_pos_y = 0.f;
	_position = vec3(0.f, 0.f, 1.f);
	_look_at = vec3(0.f, 0.f, 0.f);
	_up = vec3(0.f, 1.f, 0.f);
	_right = vec3(1.f, 0.f, 0.f);
	_window_width = 1024;
	_window_height = 768;
}

Camera::~Camera() {

}

void Camera::autoscale(Scene &s) {
	vec3 box = s.get_box_size();
	_look_at = box / 2.f;
	// this is the distance between the look_at vector and the position vector on the z axis. It is
	// computed by enforcing the requisite that the viewport should contain the whole box
	float distance = 0.5f * (1.02 * max(box[0], box[1]) / tanf(glm::radians(_fov * 0.5f)) + box[2]);
	_position = _look_at + vec3(0.f, 0.f, distance);
}

void Camera::save_state() {
	_saved_position = _position;
	_saved_look_at = _look_at;
	_saved_up = _up;
	_saved_right = _right;
	_saved_fov = _fov;
	_saved_is_ortho = _is_ortho;
}

void Camera::load_saved_state() {
	_position = _saved_position;
	_look_at = _saved_look_at;
	_up = _saved_up;
	_right = _saved_right;
	_fov = _saved_fov;
	_is_ortho = _saved_is_ortho;
}

std::string Camera::get_state() {
	std::stringstream out;

	out << "fov " << _fov << endl;
	out << "position " << _position.x << " " << _position.y << " " << _position.z << endl;
	out << "right " << _right.x << " " << _right.y << " " << _right.z << endl;
	out << "up " << _up.x << " " << _up.y << " " << _up.z << endl;
	out << "look_at " << _look_at.x << " " << _look_at.y << " " << _look_at.z << endl;
	out << "ortho " << _is_ortho << endl;
	out << "dimensions " << _window_width << " " << _window_height << endl;

	return out.str();
}

void Camera::load_state_from_file(std::ifstream &in) {
	in.clear();
	in.seekg(0);
	while(in.good()) {
		std::string buff;
		getline(in, buff);

		std::vector<std::string> spl = StringHelper::split(buff);
		if(spl.size() > 0 && buff[0] != '#') {
			if(spl[0] == "fov") {
				StringHelper::from_string<float>(spl[1], _fov);
			}
			else if(spl[0] == "position") {
				StringHelper::get_vec3(spl, 1, _position);
			}
			else if(spl[0] == "right") {
				StringHelper::get_vec3(spl, 1, _right);
			}
			else if(spl[0] == "up") {
				StringHelper::get_vec3(spl, 1, _up);
			}
			else if(spl[0] == "look_at") {
				StringHelper::get_vec3(spl, 1, _look_at);
			}
			else if(spl[0] == "ortho") {
				StringHelper::from_string<bool>(spl[1], _is_ortho);
			}
			else if(spl[0] == "dimensions") {
				StringHelper::from_string<int>(spl[1], _window_width);
				StringHelper::from_string<int>(spl[2], _window_height);
			}
		}
	}

	// all of this is needed only if the user wants to directly export to povray
	glm::normalize(_right);
	glm::normalize(_up);
	vec3 direction = glm::normalize(cross(_up, _right));
	_up = cross(_right, direction);

	_view_matrix = glm::lookAt(_position, _look_at, _up);
	_set_projection_matrix();
}

glm::mat4 Camera::ortho_matrix() {
	// we first compute the width
	float distance = glm::length(_position - _look_at);
	float half_width = distance * tanf(glm::radians(_fov / 2.f));
	// and then the height
	float half_height = half_width / window_ratio();
	return glm::ortho(-half_width, half_width, -half_height, half_height, 0.01f, 1000.0f);
}

void Camera::_set_projection_matrix() {
	if(!_is_ortho) {
		_projection_matrix = glm::perspective(_fov, window_ratio(), 0.01f, 10000.0f);
	}
	else {
		_projection_matrix = ortho_matrix();
	}
}

void Camera::update(InputState &input, float dt) {
	// rotations of the camera are controlled by the mouse or by combinations of ctrl + x or y
	float delta_horizontal_angle = input.delta_horizontal_angle;
	float delta_vertical_angle = input.delta_vertical_angle;
	bool fix_position = (delta_horizontal_angle != 0.f || delta_vertical_angle != 0.f);
	if(input.right_button) {
		if(!_right_button) {
			_right_button = true;
		}
		else {
			delta_horizontal_angle = mouse_speed * (input.cursor_pos_x - _ref_cursor_pos_x);
			delta_vertical_angle = mouse_speed * (input.cursor_pos_y - _ref_cursor_pos_y);
		}
		_ref_cursor_pos_x = input.cursor_pos_x;
		_ref_cursor_pos_y = input.cursor_pos_y;
	}
	else {
		_right_button = false;
	}

	if(input.left_button) {
		if(!_left_button) {
			_left_button = true;
		}
		else {
			delta_horizontal_angle = -mouse_speed * (input.cursor_pos_x - _ref_cursor_pos_x);
			delta_vertical_angle = -mouse_speed * (input.cursor_pos_y - _ref_cursor_pos_y);
		}
		_ref_cursor_pos_x = input.cursor_pos_x;
		_ref_cursor_pos_y = input.cursor_pos_y;
	}
	else {
		_left_button = false;
	}

	// initial direction is given by up x right
	vec3 direction = glm::normalize(cross(_up, _right));
	// we then rotate the direction along the up axis
	direction = glm::rotate(direction, glm::degrees(delta_horizontal_angle), _up);
	// we change right accordingly
	_right = glm::rotate(_right, glm::degrees(delta_horizontal_angle), _up);
	// and finally we rotate direction along the right axis
	direction = glm::rotate(direction, glm::degrees(delta_vertical_angle), _right);
	// now we can re-compute the up vector
	_up = cross(_right, direction);

	vec3 delta_pos(0.f, 0.f, 0.f);

	if(input.left) {
		delta_pos -= _right * kb_speed * dt;
	}
	if(input.right) {
		delta_pos += _right * kb_speed * dt;
	}
	if(input.up) {
		delta_pos += _up * kb_speed * dt;
	}
	if(input.down) {
		delta_pos -= _up * kb_speed * dt;
	}
	if(input.forward) {
		delta_pos += direction * delta_change_position * kb_speed * dt;
		input.forward = false;
	}
	if(input.backward) {
		delta_pos -= direction * delta_change_position * kb_speed * dt;
		input.backward = false;
	}
	if(input.fast_forward) {
		delta_pos += direction * delta_fast_change_position * kb_speed * dt;
		input.fast_forward = false;
	}
	if(input.fast_backward) {
		delta_pos -= direction * delta_fast_change_position * kb_speed * dt;
		input.fast_backward = false;
	}

	float distance = glm::length(_position - _look_at);

	if(_left_button || fix_position) {
		_position = _look_at - distance * direction;
	}
	else {
		_look_at = _position + distance * direction;
		if(distance < move_look_at_threshold) {
			_look_at += delta_pos;
		}
	}

	_position += delta_pos;

	// if f or F has been pressed
	if(input.fov_change != 0.f) {
		// field of view change
		float old_fov = _fov;
		_fov += input.fov_change;
		if(_fov <= 4.f) {
			_fov = 4.f;
		}
		if(_fov >= 180.f) {
			_fov = 179.f;
		}

		// we first compute the y-range
		float yrange = distance * tanf(glm::radians(old_fov / 2.f));
		// and then we compute the new distance by making the
		// y-range to remain the same as the fov is changed
		distance = yrange / tanf(glm::radians(_fov / 2.f));
		_position = _look_at - distance * direction;
	}

	// camera matrix
	_view_matrix = glm::lookAt(_position, _look_at, _up);

	// save the window ratio
	if(input.window_height != -1) {
		_window_height = input.window_height;
	}
	if(input.window_width != -1) {
		_window_width = input.window_width;
	}
	input.delta_horizontal_angle = input.delta_vertical_angle = 0.f;

	// projection matrix
	_set_projection_matrix();
}

void Camera::rotate(glm::vec3 axis, float angle) {
	axis = glm::normalize(axis);

	_right = glm::rotate(_right, angle, axis);
	_up = glm::rotate(_up, angle, axis);
	vec3 direction = glm::normalize(cross(_up, _right));

	float distance = glm::length(_position - _look_at);
	_position = _look_at - distance * direction;

	_view_matrix = glm::lookAt(_position, _look_at, _up);
}

void Camera::rotate_look_at_vector(glm::vec3 axis, float angle) {
	axis = glm::normalize(axis);

	_right = glm::rotate(_right, angle, axis);
	_up = glm::rotate(_up, angle, axis);
	_look_at = glm::rotate(_look_at, angle, axis);

	_view_matrix = glm::lookAt(_position, _look_at, _up);
}

void Camera::translate(glm::vec3 direction, float extent) {
	direction = glm::normalize(direction);

	_position += extent * direction;
	_look_at += extent * direction;

	_view_matrix = glm::lookAt(_position, _look_at, _up);
}

void Camera::set_is_ortho(bool v) {
	_is_ortho = v;
	Logging::log_debug("Toggling the ortographic projection to %d\n", v);
}
