/*
 * Cogli1.cpp
 *
 *  Created on: 20/ott/2013
 *      Author: lorenzo
 */

#include "Cogli2.h"

#include "cl_arguments.h"
#include "Utils/Logging.h"
#include "Parsers/MGLParser.h"
#include "Parsers/DNAParser.h"
#include "Parsers/ManfredoParser.h"
#include "Parsers/NathanParser.h"
#include "Parsers/PatchyParser.h"
#include "Parsers/StarrParser.h"
#include "Parsers/LevyParser.h"
#include "Parsers/RBCParser.h"
#include "Parsers/TEPParser.h"
#include "Parsers/ShellParser.h"
#include "Parsers/IcoParser.h"
#include "OGL/OGLManager.h"
#include "Utils/StringHelper.h"
#include "Actions/ActionBuilder.h"

#include <queue>

using namespace std;

Cogli1::Cogli1(option::Parser &parser, option::Option *options) :
				_options(options),
				_parser(NULL),
				_pov_manager(options),
				_default_visibility_state(SHOW) {
	// get the filenames
	std::queue<std::string> input_files;
	for(int i = 0; i < parser.nonOptionsCount(); ++i) {
		input_files.push(string(parser.nonOption(i)));
	}
	if(input_files.size() == 0) {
		Logging::log_critical("No input file(s) given\n");
		exit(1);
	}
	else if(options[TOPOLOGY]) {
		string topology = options[TOPOLOGY].arg;
		if(options[MANFREDO]) {
			_parser = std::make_shared<ManfredoParser>(topology, input_files);
		}
		else if(options[NATHAN]) {
			_parser = std::make_shared<NathanParser>(topology, input_files, atof(options[NATHAN].arg));
		}
		else if(options[PATCHY]) {
			_parser = std::make_shared<PatchyParser>(topology, input_files, options[PATCHY].arg);
		}
		else if(options[STARR]) {
			_parser = std::make_shared<StarrParser>(topology, input_files, options[STARR].arg);
		}
		else if(options[LEVY]) {
			_parser = std::make_shared<LevyParser>(topology, input_files);
		}
		else if(options[TEP]) {
			_parser = std::make_shared<TEPParser>(topology, input_files);
		}
		else if(options[ASHELL]) {
			_parser = std::make_shared<ShellParser>(topology, input_files);
		}
		else if(options[ICO]) {
			_parser = std::make_shared<IcoParser>(topology, input_files);
		}
		else {
			_parser = std::make_shared<DNAParser>(topology, input_files);
		}
	}
	else {
		if(options[RBC]) {
			_parser = std::make_shared<RBCParser>(input_files);
		}
		else {
			_parser = std::make_shared<MGLParser>(input_files);
		}
	}

	_parser->set_options(options);

	if(options[SHIFT_EVERY]) {
		Scene::set_shift_every(atoi(options[SHIFT_EVERY].arg));
	}
	if(options[VISIBILITY_FILE]) {
		_parse_visibility_file(options[VISIBILITY_FILE].arg);
	}
	if(options[MM_GROOVING]) {
		Nucleotide::mode = Nucleotide::MM_GROOVING;
	}
	if(options[BASE_COLOR_AS_BACKBONE] and options[BASE_COLOR_DRUMS]) {
		Logging::log_critical("--drums and --bcab options are incompatible\n");
		exit(1);
	}
	if(options[BASE_COLOR_AS_BACKBONE]) {
		Nucleotide::base_color_mode = Nucleotide::BACKBONE_COLOUR;
	}
	if(options[BASE_COLOR_DRUMS]) {
		Nucleotide::base_color_mode = Nucleotide::DRUMS_COLOUR;
	}
	if(options[RNA]) {
		Nucleotide::mode = Nucleotide::RNA;
	}

	_get_scenes();

	// only two lights are enabled at the beginning
	_lighting.toggle_light(2);
	_lighting.toggle_light(5);

	if(options[LOAD_FILE]) {
		std::ifstream dump_file(options[LOAD_FILE].arg);
		if(dump_file.bad() || !dump_file.is_open()) {
			Logging::log_critical("Dump file '%s' not accessible\n", options[LOAD_FILE].arg);
			exit(1);
		}

		try {
			_camera.load_state_from_file(dump_file);
			_lighting.load_state_from_file(dump_file);
			Scene::load_state_from_file(dump_file);
			_scenes[0]->normalise();
		}
		catch(std::string &s) {
			Logging::log_warning(s.c_str());
		}

		dump_file.close();
	}
	else {
		_camera.autoscale(*_scenes[0]);
	}

	_centre.always_centre = options[ALWAYS_CENTRE];
	if(options[ALWAYS_CENTRE].arg != NULL) {
		_centre.centre_on = atoi(options[ALWAYS_CENTRE].arg);
	}
	_centre.consider_inertia = options[INERTIA];

	if(options[COM_CENTRE] || _centre.always_centre) {
		_scenes[0]->centre(_centre);
	}

	// load groups
	if(options[GROUPS_FROM]) {
		std::ifstream groups_file(options[GROUPS_FROM].arg);
		if(groups_file.bad() || !groups_file.is_open()) {
			Logging::log_critical("Could not access file '%s' to load groups from\n", options[GROUPS_FROM].arg);
			exit(1);
		}
	}
}

Cogli1::~Cogli1() {

}

void Cogli1::_parse_visibility_file(const char *v_file) {
	ifstream v(v_file);

	if(!v.good()) {
		Logging::log_critical("Visibility file '%s' not readable\n", v_file);
		exit(1);
	}

	// the first line might optionally be default:token, where token should be either 'show' or 'hide'
	std::string line;
	std::getline(v, line);
	StringHelper::to_lower(line);
	auto spl = StringHelper::split(line, ':');
	Visibility default_status_to_apply = HIDE;
	if(spl.size() > 1 && spl[0] == "default") {
		if(spl[1] == "show") {
			_default_visibility_state = SHOW;
		}
		else if(spl[1] == "hide") {
			_default_visibility_state = HIDE;
			default_status_to_apply = SHOW;
		}
		else {
			Logging::log_critical("Invalid keyword '%s' for the visibility default state; use either 'show' or 'hide'.", spl[1].c_str());
			exit(1);
		}
	}
	else {
		v.seekg(0, std::ios::beg);
	}

	while(v.good()) {
		Visibility line_default = default_status_to_apply;

		std::getline(v, line);
		StringHelper::to_lower(StringHelper::trim(line));

		if(line.size() > 0) {
			std::string to_parse = line;
			auto spl = StringHelper::split(line, ':');
			if(spl.size() > 1) {
				if(spl[0] == "show") {
					line_default = SHOW;
				}
				else if(spl[0] == "hide") {
					line_default = HIDE;
				}
				else {
					Logging::log_critical("Invalid keyword '%s' for the visibility state of line '%s'; use either 'show' or 'hide'.", spl[0].c_str(), line.c_str());
					exit(1);
				}
				to_parse = spl[1];
			}
			auto ids = StringHelper::ids_from_range(to_parse);
			for(auto id : ids) {
				_invisibile_particles[id] = line_default;
			}
		}
	}

	v.close();
}

void Cogli1::_get_scenes() {
	glm::vec3 box;
	if(_options[BOX]) {
		vector<string> spl = StringHelper::split(string(_options[BOX].arg), ',');
		if(spl.size() != 3) {
			Logging::log_critical("The argument passed to --box should be composed of three numbers separated by commas and not contain any spaces (e.g. 10,5,10)\n");
			exit(1);
		}
		StringHelper::get_vec3(spl, 0, box);
	}
	int to_skip = 0;
	if(_options[CONFS_TO_SKIP]) to_skip = atoi(_options[CONFS_TO_SKIP].arg);

	int i = 0;
	while(!_parser->done()) {
		std::shared_ptr<Scene> new_scene = std::shared_ptr<Scene>(_parser->next_scene());
		if(_options[BOX]) {
			new_scene->set_box(box);
		}
		new_scene->set_id(i);
		if(_options[BRING_IN_BOX]) {
			new_scene->bring_in_box();
		}

		vector<Shape *> shapes = new_scene->get_shapes();
		bool visible = (_default_visibility_state == SHOW);
		for(auto shape : shapes) {
			shape->set_visible(visible);
		}

		for(auto pair : _invisibile_particles) {
			int nid = pair.first;
			visible = (pair.second == SHOW);
			if(nid < (int) shapes.size()) {
				shapes[nid]->set_visible(visible);
				if(_options[OPACITY] && !visible) {
					shapes[nid]->set_opacity(atof(_options[OPACITY].arg));
				}
			}
		}

		if(_options[ACTION_FILE]) {
			auto actions = ActionBuilder::actions_from_file(string(_options[ACTION_FILE].arg), _camera);
			new_scene->set_actions(actions);
		}

		_scenes.push_back(new_scene);

		i++;
		_parser->skip(to_skip);
	}
}

void Cogli1::start_ogl() {
	OGLManager manager(_options, _centre);

	try {
		manager.init(_camera, _lighting, _scenes);
		while(!manager.done()) {
			manager.draw();
			if(manager.print_pov()) {
				manager.set_print_pov(false);
				Scene *s = manager.current_scene();
				_pov_manager.print_scene(*s, _camera, _lighting);
			}
		}
		manager.clean();
	}
	catch(string &error) {
		Logging::log_critical(error);
		exit(1);
	}
}

void Cogli1::print_all_pov() {
	for(auto scene : _scenes) {
		if(_options[ALWAYS_CENTRE]) {
			scene->centre(_centre);
		}
		else scene->normalise();

		do {
			_pov_manager.print_scene(*scene, _camera, _lighting);
		} while(scene->next_action());
	}
}
