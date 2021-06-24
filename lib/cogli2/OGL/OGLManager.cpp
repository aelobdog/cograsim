/*
 * OGLManager.cpp
 *
 *  Created on: 26/ott/2013
 *      Author: lorenzo
 */

#define GLM_SWIZZLE

#include "OGLManager.h"

#include "utils.h"
#include "../Utils/Logging.h"
#include "../cl_arguments.h"
#include "../Utils/StringHelper.h"

#include <glm/gtx/transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glm/gtx/norm.hpp>

#include <fstream>

#ifdef ENABLE_PNG_EXPORT
#include <png.h>
#endif

#include <cmrc/cmrc.hpp>
CMRC_DECLARE (assets);

#define GLT_IMPLEMENTATION
#define GLT_MANUAL_VIEWPORT
#include <gltext/gltext.h>

using std::string;
using std::vector;

OGLManager::OGLManager(option::Option *options, CentreOptions &centre) :
				_window(NULL),
				_camera(NULL),
				_lighting(NULL),
				_options(options),
				_centre(centre) {
	_input.reset();

	_show_info = false;

	// for the FPS counter
	_fps_last_time = 0.;
	_current_time = 0.;
	_last_time = 0.;
	_print_pov = false;
	_is_next_scene = false;
	_is_previous_scene = false;
	if(options[GROUPS_FROM]) {
		_load_groups = true;
		_load_groups_from.open(options[GROUPS_FROM].arg);
	}
	else {
		_load_groups = false;
	}
}

OGLManager::~OGLManager() {
	gltTerminate();
}

void OGLManager::init(Camera &camera, LightingState &lighting, std::vector<std::shared_ptr<Scene>> &scenes) {
	_camera = &camera;
	_lighting = &lighting;
	_scenes = scenes;

	_current_scene = _scenes.begin();

	(*_current_scene)->load_groups_from_file(_load_groups_from);

	// Initialise GLFW
	if(!glfwInit()) {
		throw string("Failed to initialize GLFW\n");
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// get window's width and height
	camera.window_dimensions(_input.window_width, _input.window_height);
	if(_options[DIM]) {
		string wxh = string(_options[DIM].first()->arg);
		vector<string> spl_wxh = StringHelper::split(wxh, 'x');
		if(spl_wxh.size() != 2)
			Logging::log_warning("Invalid argument supplied for --dim, defaulting to 1024x768");
		else {
			StringHelper::from_string<int>(spl_wxh[0], _input.window_width);
			StringHelper::from_string<int>(spl_wxh[1], _input.window_height);
		}
	}
	// open a window and create its OpenGL context
	_window = glfwCreateWindow(_input.window_width, _input.window_height, (*_current_scene)->get_title().c_str(), NULL, NULL);
	if(_window == NULL) {
		glfwTerminate();
		throw string("Failed to open GLFW window\n");
	}
	// initialise the opengl context
	glfwMakeContextCurrent(_window);
	// associate this object to the window that we just initialised. In this way callbacks
	// will have access to this object
	glfwSetWindowUserPointer(_window, (void*) this);

	// initialise GLAD
	if(!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
		throw string("Failed to initialize GLAD\n");
	}

	if(!gltInit()) {
		glfwTerminate();
		throw string("Failed to initialize glText\n");
	}
	GLint m_viewport[4];
	glGetIntegerv(GL_VIEWPORT, m_viewport);
	gltViewport(m_viewport[2], m_viewport[3]);

	// initialise callbacks
	glfwSetKeyCallback(_window, OGLManager::key_callback);
	glfwSetMouseButtonCallback(_window, OGLManager::mouse_button_callback);
	glfwSetScrollCallback(_window, OGLManager::scroll_callback);
	glfwSetCursorPosCallback(_window, OGLManager::cursor_pos_callback);
	glfwSetCursorEnterCallback(_window, OGLManager::cursor_enter_callback);
	glfwSetWindowSizeCallback(_window, OGLManager::window_resize_callback);
	glfwSetFramebufferSizeCallback(_window, OGLManager::framebuffer_resize_callback);

	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glEnable(GL_DEPTH_TEST);
	glCheckError();

	glEnable(GL_BLEND);
	glCheckError();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glCheckError();

	if(_options[DEBUG]) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	_camera->save_state();

	if(_options[RESOLUTION]) {
		int res;
		StringHelper::from_string<int>(_options[RESOLUTION].arg, res);
		_drawer.set_resolution(res);
	}

	auto fs = cmrc::assets::get_filesystem();

	auto infile = fs.open("OGL/Assets/Shaders/shape.vs");
	std::string shape_vertex((infile.begin()), (infile.end()));

	infile = fs.open("OGL/Assets/Shaders/shape.fs");
	std::string shape_fragment((infile.begin()), (infile.end()));
	_shape_shader = std::make_shared<Shader>(Shader(shape_vertex, shape_fragment));
	_shape_shader->set_bool("disable_lighting", false);

	infile = fs.open("OGL/Assets/Shaders/axes.vs");
	std::string axes_vertex((infile.begin()), (infile.end()));

	infile = fs.open("OGL/Assets/Shaders/axes.fs");
	std::string axes_fragment((infile.begin()), (infile.end()));
	_axes_shader = std::make_shared<Shader>(Shader(axes_vertex, axes_fragment));

	_drawer.initialise();
	_drawer.set_shader(_shape_shader);
}

void OGLManager::update_lights(std::shared_ptr<Shader> shader) {
	shader->set_vec3("ambient_light", _lighting->ambient);

	auto lights = _lighting->get_lights();
	for(unsigned int i = 0; i < lights.size(); i++) {
		auto light = lights[i];

		std::string base_str = StringHelper::sformat("pointLights[%d].", i);

		shader->set_bool(base_str + "enabled", light.enabled);
		shader->set_vec3(base_str + "direction", light.direction);
		shader->set_vec3(base_str + "diffuse", light.diffuse);
		_shape_shader->set_vec3(base_str + "specular", light.specular);
	}
}

void OGLManager::_compute_fps() {
	if(_fps_last_time == 0.) {
		_fps_last_time = glfwGetTime();
	}
	_n_frames++;

	// update each second
	if(_current_time - _fps_last_time >= 1.0) {
		_current_fps = _n_frames;
		_n_frames = 0;
		_fps_last_time += 1.0;
	}
}

void OGLManager::_render(bool force_scene_update) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	_shape_shader->use();

	if(_lighting->dirty()) {
		update_lights(_shape_shader);
		_lighting->set_dirty(false);
	}

	_shape_shader->set_mat4("projection", _camera->get_projection_matrix());
	_shape_shader->set_mat4("view", _camera->get_view_matrix());
	_shape_shader->set_vec3("camera_position", _camera->position());

	_drawer.set_camera_position(_camera->position());

	if((*_current_scene)->dirty() || force_scene_update) {
		(*_current_scene)->accept_drawer(_drawer);
	}
	_drawer.draw();

	if(_show_axes) {
		_axes_shader->use();
		update_lights(_axes_shader);

		glm::mat4 projection_matrix = glm::perspective(45.f, _camera->window_ratio(), 0.01f, 10000.0f);
		glm::vec3 camera_direction = glm::normalize(_camera->position() - _camera->look_at());
		glm::vec3 axes_camera_pos = 8.f * camera_direction;
		glm::mat4 axes_view = glm::lookAt(axes_camera_pos, glm::vec3(0.f, 0.f, 0.f), _camera->up());
		// this is applied after the projection matrix, when coordinates have been converted to the [-1, 1] range
		glm::vec2 translation_offset(-0.75f, -0.75f);

		_axes_shader->set_mat4("projection", projection_matrix);
		_axes_shader->set_mat4("view", axes_view);
		_axes_shader->set_vec3("camera_position", axes_camera_pos);
		_axes_shader->set_vec2("translation_offset", translation_offset);

		glDisable(GL_DEPTH);
		_drawer.draw_axes_arrows(_axes_shader);
		glEnable(GL_DEPTH);
	}

	if(_show_info) {
		if(_current_fps != -1) {
			glm::vec2 coords(50.f, 50.f);

			// Creating text
			GLTtext *text = gltCreateText();
			std::string fps = StringHelper::sformat("%d fps", _current_fps);
			gltSetText(text, fps.c_str());

			gltBeginDraw();

			gltColor(1.0f, 1.0f, 1.0f, 1.0f);
			gltDrawText2D(text, coords.x, coords.y, 3.f);

			gltEndDraw();

			// Deleting text
			gltDeleteText(text);
		}
	}

	if(_show_labels && !_camera->is_ortho()) {
		GLint m_viewport[4];
		glGetIntegerv(GL_VIEWPORT, m_viewport);

		auto camera_pos = _camera->position();
		glm::mat4 vp_matrix = _camera->get_projection_matrix() * _camera->get_view_matrix();

		auto &shapes = current_scene()->get_sortable_shapes();
		auto sort_lambda = [camera_pos](const Shape *s1, const Shape *s2) {
			return glm::length2(camera_pos - s1->position()) < glm::length2(camera_pos - s2->position());
		};
		std::sort(shapes.begin(), shapes.end(), sort_lambda);
		gltBeginDraw();
		gltColor(1.0f, 1.0f, 1.0f, 1.0f);
		GLTtext *text = gltCreateText();

		for(auto shape : shapes) {
			if(shape->is_visible()) {
				std::string to_render = StringHelper::sformat("%d", shape->get_id());

				glm::vec4 shape_position(shape->position(), 1.f);
				// clip space coordinates
				glm::vec4 coordinates(vp_matrix * shape_position);
				// normalized device coordinates
				glm::vec2 screen_coordinates(coordinates / coordinates.w);
				// screen coordinates
				screen_coordinates.x = ((screen_coordinates.x + 1.f) / 2.) * m_viewport[2] + m_viewport[0];
				screen_coordinates.y = m_viewport[3] - ((screen_coordinates.y + 1.f) / 2.) * m_viewport[3] + m_viewport[1];
				float scale = 25.f / coordinates.w;
				gltSetText(text, to_render.c_str());

				gltDrawText2D(text, screen_coordinates.x, screen_coordinates.y, scale);
			}
		}

		gltDeleteText(text);
		gltEndDraw();
	}
}

void OGLManager::draw() {
	if(_is_next_scene || _is_previous_scene) {
		if(!_centre.always_centre) {
			(*_current_scene)->normalise();
		}
		glfwSetWindowTitle(_window, (*_current_scene)->get_title().c_str());
	}

	_is_next_scene = _is_previous_scene = false;

	if(_show_info) {
		_compute_fps();
	}

	_current_time = glfwGetTime();
	float dt = float(_current_time - _last_time);
	_camera->update(_input, dt);
	_input.fov_change = 0.f;

	_last_time = _current_time;

	_render();

	glfwSwapBuffers(_window);
	glfwPollEvents();
}

bool OGLManager::done() {
	return glfwWindowShouldClose(_window);
}

void OGLManager::previous_scene(int offset) {
	_is_previous_scene = true;

	// apply actions first
	bool not_done = true;
	do {
		not_done = (*_current_scene)->previous_action();
		if(not_done) {
			offset--;
		}
	} while(not_done && offset);

	int current_scene_index = &*_current_scene - &*_scenes.begin();

	if((current_scene_index - offset) >= 0) {
		_current_scene -= offset;
		if(_centre.always_centre) {
			(*_current_scene)->centre(_centre);
		}
	}
}

void OGLManager::next_scene(int offset) {
	_is_next_scene = true;

	// apply actions first
	bool not_done = true;
	do {
		not_done = (*_current_scene)->next_action();
		if(not_done) {
			offset--;
		}
	} while(not_done && offset);

	int current_scene_index = &*_current_scene - &*_scenes.begin();
	if((unsigned int) (current_scene_index + offset) < _scenes.size()) {
		_current_scene += offset;
		if(_centre.always_centre) {
			(*_current_scene)->centre(_centre);
		}
	}
}

void OGLManager::pick(bool print_only) {
	_drawer.toggle_selection_mode();
	_shape_shader->use();
	_shape_shader->set_bool("disable_lighting", true);

	// labels are not to be rendered in picking mode
	bool old_show_labels = _show_labels;
	if(_show_labels) {
		_show_labels = false;
	}
	glDisable(GL_BLEND);
	_render(true);

	// http://www.opengl-tutorial.org/miscellaneous/clicking-on-objects/picking-with-an-opengl-hack/
	glFlush();
	glFinish();
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	int width, height;
	glfwGetFramebufferSize(_window, &width, &height);

	unsigned char color[4];

	int x = _input.cursor_pos_x;
	int y = _input.cursor_pos_y;

	glReadPixels(x, height - y - 1, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, color);

	int picked = color[0] + color[1] * 256 + color[2] * 256 * 256;
	// if all three components are FF then the user clicked on the background or on some
	// other non-particle shape
	if(picked == 0x00ffffff) {
		picked = -1;
	}

	Logging::log_debug("Clicked on pixel %d, %d, color %02hhx%02hhx%02hhx%02hhx, particle index %d\n", x, y, color[0], color[1], color[2], color[3], picked);

	if(old_show_labels) {
		_show_labels = true;
	}

	glEnable(GL_BLEND);
	_drawer.toggle_selection_mode();
	_shape_shader->use();
	_shape_shader->set_bool("disable_lighting", false);
	_render(true);

	if(print_only) {
		if(picked == -1) {
			Logging::log_info("You clicked on the background or on a non-pickable object\n");
		}
		else {
			Logging::log_info("You clicked on object n. %d\n", picked);
		}
	}
	else if(picked != -1) {
		Logging::log_info("Selected object n. %d\n", picked);
		(*_current_scene)->select_shape(picked);
	}
}

void OGLManager::toggle_info() {
	_show_info = !_show_info;
	if(_show_info) {
		_fps_last_time = 0.;
	}
}

#ifdef ENABLE_PNG_EXPORT
void OGLManager::export_png() {
	// we first take out the : in the scene's title and then we substitute any space with underscores
	std::vector<string> spl = StringHelper::split((*_current_scene)->get_title(), ':');
	string filename = StringHelper::join(spl, "");
	std::replace(filename.begin(), filename.end(), ' ', '_');
	filename += ".png";

	Logging::log_info("Exporting current view on file '%s'\n", filename.c_str());

	// we first read the raw pixel values
	int width, height;
	glfwGetFramebufferSize(_window, &width, &height);

	int n_pixels = width * height * 3;
	GLubyte *pixels = new GLubyte[n_pixels];
	glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);

	// then we use libpng to print out the image file
	png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	png_infop info_ptr = png_create_info_struct(png_ptr);
	int pixel_size = 3;
	int depth = 8;

	png_set_IHDR(png_ptr,
			info_ptr,
			width,
			height,
			depth,
			PNG_COLOR_TYPE_RGB,
			PNG_INTERLACE_NONE,
			PNG_COMPRESSION_TYPE_DEFAULT,
			PNG_FILTER_TYPE_DEFAULT);

	// initialise rows of PNG
	png_byte **row_pointers = (png_byte **) png_malloc(png_ptr, height * sizeof(png_byte *));
	for(int y = 0; y < height; ++y) {
		png_byte *row = (png_byte *) png_malloc(png_ptr, sizeof(uint8_t) * width * pixel_size);
		// for mysterious reasons rows are upside down
		row_pointers[height - 1 - y] = row;
		for(int x = 0; x < width; ++x) {
			*row++ = pixels[3*y*width + 3*x + 0];
			*row++ = pixels[3*y*width + 3*x + 1];
			*row++ = pixels[3*y*width + 3*x + 2];
		}
	}

	// write the data
	FILE *out = fopen(filename.c_str(), "w");

	png_init_io(png_ptr, out);
	png_set_rows(png_ptr, info_ptr, row_pointers);
	png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

	fclose(out);

	// free memory
	for(int y = 0; y < height; y++) png_free (png_ptr, row_pointers[y]);
	png_free(png_ptr, row_pointers);

	png_destroy_write_struct (&png_ptr, &info_ptr);

	Logging::log_info("done\n");
}
#endif

void OGLManager::change_ambient(float delta) {
	_lighting->change_ambient(delta);
}

void OGLManager::clean() {
	if(_window != NULL)
		glfwDestroyWindow(_window);
	glfwTerminate();
}

void OGLManager::key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	OGLManager *manager = (OGLManager*) glfwGetWindowUserPointer(window);
	InputState &input = manager->_input;

	bool ctrl = (mods & GLFW_MOD_CONTROL);
	bool shift = (mods & GLFW_MOD_SHIFT);
	bool release = (action == GLFW_RELEASE);
	bool press = (action == GLFW_PRESS);

	// move the camera by using arrow keys
	if(key == GLFW_KEY_UP) {
		input.up = !release;
	}
	else if(key == GLFW_KEY_DOWN) {
		input.down = !release;
	}
	else if(key == GLFW_KEY_LEFT) {
		input.left = !release;
	}
	else if(key == GLFW_KEY_RIGHT) {
		input.right = !release;
	}
	// change the fov
	else if(key == GLFW_KEY_F) {
		if(release) {
			input.fov_change = 0.f;
		}
		else {
			if(shift) {
				input.fov_change = manager->delta_fov;
			}
			else {
				input.fov_change = -manager->delta_fov;
			}
		}
	}
	else if(key == GLFW_KEY_I && press) {
		manager->toggle_info();
	}
	// change the current configuration
	else if((key == GLFW_KEY_KP_ADD || key == ']') && !release) {
		if(ctrl) {
			manager->next_scene(10);
		}
		else {
			manager->next_scene();
		}
	}
	else if((key == GLFW_KEY_KP_SUBTRACT || key == '/') && !release) {
		if(ctrl) {
			manager->previous_scene(10);
		}
		else {
			manager->previous_scene();
		}
	}
	// in order to keep things tidy, instead of adding 8 cases to the next switch we manage lights
	// in this weird way, hoping that osg will keep F1-F8 key values one after the other
	else if(key >= GLFW_KEY_F1 && key <= GLFW_KEY_F8 && press) {
		unsigned int num_light = key - GLFW_KEY_F1;
		manager->_lighting->toggle_light(num_light);
	}
	// amount of ambient light
	else if(key == GLFW_KEY_A && !release) {
		// if shift is being pressed than we decrease the ambient light
		float delta = (shift) ? -LIGHT_DELTA_AMBIENT : LIGHT_DELTA_AMBIENT;

		manager->_lighting->change_ambient(delta);
	}
	// amount of diffused light
	else if(key == GLFW_KEY_D && !release) {
		// if shift is being pressed than we decrease the diffused light
		float delta = (shift) ? -LIGHT_DELTA_DIFFUSE : LIGHT_DELTA_DIFFUSE;

		manager->_lighting->change_diffuse(delta);
	}
	// manage the saving/loading of the current state of the camera
	else if(key == GLFW_KEY_C && press) {
		if(shift) {
			manager->_camera->save_state();
		}
		else {
			manager->_camera->load_saved_state();
		}
	}
	// manage resolution changes
	else if(key == GLFW_KEY_PAGE_UP && !release) {
		manager->_drawer.increase_resolution();
		manager->current_scene()->set_dirty(true);
	}
	else if(key == GLFW_KEY_PAGE_DOWN && !release) {
		manager->_drawer.decrease_resolution();
		manager->current_scene()->set_dirty(true);
	}
	// shift or rotate along x
	else if(key == GLFW_KEY_X && !release) {
		if(ctrl) {
			input.delta_horizontal_angle = (shift) ? manager->delta_angle : -manager->delta_angle;
		}
		else {
			glm::vec3 x_shift(manager->delta_shift, 0.f, 0.f);
			if(shift) {
				x_shift[0] = -manager->delta_shift;
			}
			(*manager->_current_scene)->translate(x_shift);
		}
	}
	// shift or rotate along y
	else if(key == GLFW_KEY_Y && !release) {
		if(ctrl) {
			input.delta_vertical_angle = (shift) ? manager->delta_angle : -manager->delta_angle;
		}
		else {
			glm::vec3 y_shift(0.f, manager->delta_shift, 0.f);
			if(shift) {
				y_shift[1] = -manager->delta_shift;
			}
			(*manager->_current_scene)->translate(y_shift);
		}
	}
	// shift along z
	else if(key == GLFW_KEY_Z && !release) {
		glm::vec3 z_shift(0.f, 0.f, manager->delta_shift);
		if(shift) {
			z_shift[2] = -manager->delta_shift;
		}
		(*manager->_current_scene)->translate(z_shift);
	}
	// toggle box's visibility
	else if(key == GLFW_KEY_B && !release) {
		(*manager->_current_scene)->toggle_box();
	}
	// toggle the axes' arrows visibility
	else if(key == GLFW_KEY_K && !release) {
		manager->_show_axes = !manager->_show_axes;
	}
	// toggle the axes' arrows visibility
	else if(key == GLFW_KEY_L && !release) {
		manager->_show_labels = !manager->_show_labels;
	}
	else if(key == GLFW_KEY_P && shift && press) {
		manager->_print_pov = true;
	}
	// dump system's state to a file
	else if(key == GLFW_KEY_U && press) {
		// we first take out the : in the scene's title and then we substitute any space with underscores
		std::vector<string> spl = StringHelper::split(manager->current_scene()->get_title(), ':');
		string filename = StringHelper::sanitize_filename(StringHelper::join(spl, ""));
		filename += ".cpy";

		std::ofstream out(filename.c_str());

		out << manager->_camera->get_state();
		out << Scene::get_state();
		out << manager->_lighting->get_state();

		Logging::log_info("System state printed on '%s'\n", filename.c_str());
	}
	// zoom in/out
	else if(key == GLFW_KEY_Q && !release) {
		if(ctrl) {
			if(shift) {
				manager->_input.fast_backward = true;
			}
			else {
				manager->_input.fast_forward = true;
			}
		}
		else {
			if(shift) {
				manager->_input.backward = true;
			}
			else {
				manager->_input.forward = true;
			}
		}
	}
	else if(key == GLFW_KEY_V && press) {
		if(shift) {
			CentreOptions centre = manager->_centre;
			centre.consider_inertia = true;
			manager->current_scene()->centre(centre);
		}
		else {
			manager->current_scene()->centre(manager->_centre);
		}
	}
	// export the current view to a PNG file
	else if(key == GLFW_KEY_P && ctrl && press) {
#ifdef ENABLE_PNG_EXPORT
		manager->export_png();
#endif
	}
	// toggle ortographic projection
	else if(key == GLFW_KEY_P && press) {
		manager->_camera->set_is_ortho(!manager->_camera->is_ortho());
	}
	else if(key == GLFW_KEY_W && ctrl && press) {
		// invert selection
		if(shift) {
			manager->current_scene()->invert_selection();
		}
		// select all particles
		else {
			manager->current_scene()->select_all_shapes();
		}
	}
	// hide selected particles
	else if(key == GLFW_KEY_H && press) {
		manager->current_scene()->hide_selected();
	}
	// mark as visible all the shapes
	else if(key == GLFW_KEY_S && press) {
		manager->current_scene()->show_all_shapes();
	}
	// manage control groups
	else if(key >= GLFW_KEY_0 && key <= GLFW_KEY_9 && press) {
		int g_id = key - GLFW_KEY_0;
		if(ctrl) {
			manager->current_scene()->set_selected_as_group(g_id);
		}
		else if(!shift) {
			manager->current_scene()->select_group(g_id);
		}
	}
	// print control groups
	else if(key == GLFW_KEY_G && shift && press) {
		// we build the filename
		std::vector<string> spl = StringHelper::split(manager->current_scene()->get_title(), ':');
		string filename = StringHelper::join(spl, "");
		std::replace(filename.begin(), filename.end(), ' ', '_');
		filename += ".grcpy";
		// stampiamo i control groups
		std::ofstream out(filename.c_str());
		out << manager->current_scene()->print_control_groups();
		Logging::log_info("Groups printed to '%s'\n", filename.c_str());
	}
}

void OGLManager::mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
	OGLManager *manager = (OGLManager*) glfwGetWindowUserPointer(window);
	InputState &input = manager->_input;

	bool ctrl = (mods & GLFW_MOD_CONTROL);
	bool shift = (mods & GLFW_MOD_SHIFT);

	if(button == GLFW_MOUSE_BUTTON_LEFT) {
		// if the left button is pressed while ctrl is held down then we enter picking mode
		if(action == GLFW_RELEASE) {
			if(ctrl) {
				manager->pick();
			}
			if(shift) {
				manager->pick(true);
			}
		}
		// otherwise we signal that the left button was pressed
		input.left_button = (action == GLFW_PRESS && !ctrl && !shift);
	}
	else if(button == GLFW_MOUSE_BUTTON_MIDDLE) {
		input.middle_button = (action == GLFW_PRESS);
	}
	else if(button == GLFW_MOUSE_BUTTON_RIGHT) {
		input.right_button = (action == GLFW_PRESS);
	}
}

void OGLManager::scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
	OGLManager *manager = (OGLManager*) glfwGetWindowUserPointer(window);
	InputState &input = manager->_input;

	input.forward = input.backward = false;

	if(yoffset > 0.f)
		input.forward = true;
	if(yoffset < 0.f)
		input.backward = true;
}

void OGLManager::cursor_pos_callback(GLFWwindow *window, double xpos, double ypos) {
	OGLManager *manager = (OGLManager*) glfwGetWindowUserPointer(window);
	InputState &input = manager->_input;

	input.cursor_pos_x = xpos;
	input.cursor_pos_y = ypos;
}

void OGLManager::cursor_enter_callback(GLFWwindow *window, int entered) {
	// if the cursor leaves the window
	if(entered == GL_FALSE) {

	}
}

void OGLManager::window_resize_callback(GLFWwindow *window, int width, int height) {
	OGLManager *manager = (OGLManager*) glfwGetWindowUserPointer(window);
	InputState &input = manager->_input;

	input.window_height = height;
	input.window_width = width;
}

void OGLManager::framebuffer_resize_callback(GLFWwindow *window, int width, int height) {
	glViewport(0, 0, width, height);
	gltViewport(width, height);
}
