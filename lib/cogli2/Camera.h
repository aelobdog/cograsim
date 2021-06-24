/*
 * Camera.h
 *
 *  Created on: 27/ott/2013
 *      Author: lorenzo
 */

#ifndef CAMERA_H_
#define CAMERA_H_

#include <glm/glm.hpp>
#include "Drawables/Scene.h"

typedef struct {
	bool left;
	bool right;
	bool up;
	bool down;
	bool forward;
	bool backward;
	bool left_button;
	bool middle_button;
	bool right_button;
	bool fast_forward;
	bool fast_backward;

	float cursor_pos_x, cursor_pos_y;
	float delta_horizontal_angle, delta_vertical_angle;

	float fov_change;

	int window_height;
	int window_width;

	void reset() {
		left = right = false;
		up = down = false;
		forward = backward = fast_forward = fast_backward = false;
		left_button = right_button = middle_button = false;
		cursor_pos_x = cursor_pos_y = 0.f;
		fov_change = 0.f;
		window_width = window_height = -1;
		delta_horizontal_angle = delta_vertical_angle = 0.f;
	}
} InputState;

class Camera {
protected:
	glm::vec3 _position;
	glm::vec3 _look_at;
	glm::vec3 _up;
	glm::vec3 _right;

	glm::vec3 _saved_position;
	glm::vec3 _saved_look_at;
	glm::vec3 _saved_up;
	glm::vec3 _saved_right;
	float _saved_fov;
	bool _saved_is_ortho;

	glm::mat4 _view_matrix;
	bool _is_ortho;
	glm::mat4 _projection_matrix;

	float _fov;

	bool _left_button;
	bool _right_button;
	float _ref_cursor_pos_x;
	float _ref_cursor_pos_y;

	int _window_height;
	int _window_width;

	void _set_projection_matrix();
public:
	const float move_look_at_threshold = 2.f;
	const float delta_change_position = 10.f;
	const float delta_fast_change_position = 50.f;
	const float kb_speed = 5.f;
	const float mouse_speed = 0.005f;

	Camera();
	virtual ~Camera();

	void autoscale(Scene &s);
	void update(InputState &input, float dt);
	void rotate(glm::vec3 axis, float angle);
	void rotate_look_at_vector(glm::vec3 axis, float angle);
	void translate(glm::vec3 direction, float extent);

	glm::vec3 position() { return _position; }
	glm::vec3 right() { return _right; }
	glm::vec3 up() { return _up; }
	glm::vec3 look_at() { return _look_at; }
	float window_ratio() { return _window_width/(float)_window_height; }
	void window_dimensions(int &width, int &height) {
		width = _window_width;
		height = _window_height;
	}
	float fov() { return _fov; }
	bool is_ortho() { return _is_ortho; }

	void set_is_ortho(bool v);

	void save_state();
	void load_saved_state();

	std::string get_state();
	void load_state_from_file(std::ifstream &in);

	glm::mat4 ortho_matrix();

	glm::mat4 &get_view_matrix() { return _view_matrix; }
	glm::mat4 &get_projection_matrix() { return _projection_matrix; }
};

#endif /* CAMERA_H_ */
