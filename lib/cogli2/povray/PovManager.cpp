/*
 * PovManager.cpp
 *
 *  Created on: 07/nov/2013
 *      Author: lorenzo
 */

#include <algorithm>
#include <cstdlib>
#include <vector>

#include "PovManager.h"
#include "../Utils/StringHelper.h"
#include "../Utils/MathHelper.h"
#include "../Utils/Logging.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include "../cl_arguments.h"

using std::string;
using glm::vec3;
using glm::dvec3;
using glm::vec4;
using glm::mat4;
using glm::dmat4;

PovManager::PovManager(option::Option *options) : _options(options) {
	if(options[DNA_RIBBONS]) _drawer.set_DNA_as_ribbon(true);
	if(options[PATCHY_KF]) _drawer.set_patchy_as_kf(true);
	if(options[DRAW_OUTLINES]) _drawer.draw_outlines(true);
	if(options[POVRAY_SHADOWS]) _drawer.cast_shadows(true);
}

PovManager::~PovManager() {

}

void PovManager::print_scene(Scene &scene, Camera &camera, LightingState &lights) {
	// we first take out the : in the scene's title and then we substitute any space with underscores
	string filename = StringHelper::sanitize_filename(scene.get_title());
	filename += ".pov";

	Logging::log_info("Dumping povray output for scene '%s' on file '%s'\n", scene.get_title().c_str(), filename.c_str());

	FILE *out = fopen(filename.c_str(), "w");

	fprintf(out, "// switches for output\n");
	fprintf(out, "#declare DRAW_BASES = 1; // possible values are 0, 1; only relevant for DNA ribbons\n");
	fprintf(out, "#declare DRAW_BASES_TYPE = 3; // possible values are 1, 2, 3; only relevant for DNA ribbons\n");
	fprintf(out, "#declare DRAW_FOG = 0; // set to 1 to enable fog\n");

	fprintf(out, "\n#include \"colors.inc\"\n");
	fprintf(out, "\n#include \"transforms.inc\"\n");

	fprintf(out, "background { rgb <1, 1, 1>}\n\n");

	float ambient = 1.;
	if(_options[POVRAY_SHADOWS]) {
		ambient = 0.1;
	}
	float ambient_multiplier = 1. / ambient;
	float diffuse = lights.get_light(0).diffuse[0];
	float specular = lights.get_light(0).specular[0];
	specular = 0.; // it looks better without specular reflection
	fprintf(out, "#default {\n"
			"   normal{\n"
			"       ripples 0.25\n"
			"       frequency 0.20\n"
			"       turbulence 0.2\n"
			"       lambda 5\n"
			"   }\n"
			"	finish {\n"
			"		ambient %f\n"
			"		diffuse %f\n"
			"		specular %f\n"
			"	}\n"
	"}\n", ambient, diffuse, specular);

	vec3 right = camera.right();
	vec3 eye = camera.position();
	vec3 up = camera.up();
	vec3 look_at = camera.look_at();

	float f1 = camera.window_ratio();
	float f2 = 2.f*atanf(tanf(glm::radians(camera.fov())/2.f) * f1) / glm::radians(camera.fov());

	int width, height;
	camera.window_dimensions(width, height);
	Logging::log_info("Dimensions: %dx%d, ratio: %g +H%d +W%d -- %g\n", width, height, f1, 500, (int) (500 * f1), f2);

	fprintf(out, "\n// original window dimensions: %dx%d\n\n", width, height);
	fprintf(out, "\n// camera settings\n\n");
	fprintf(out, "camera {\n");
	if(camera.is_ortho()) fprintf(out, "\torthographic\n");
	fprintf(out, "\tsky <%g, %g, %g>\n", up[0], up[1], up[2]);
	fprintf(out, "\tup <%g, %g, %g>\n", up[0], up[1], up[2]);
	fprintf(out, "\tright %g * <%g, %g, %g>\n", f1, right[0], right[1], right[2]);
	fprintf(out, "\tlocation <%g, %g, %g>\n", eye[0], eye[1], eye[2]);
	fprintf(out, "\tlook_at <%g, %g, %g>\n", look_at[0], look_at[1], look_at[2]);
	vec3 direction = look_at - eye;
	fprintf(out, "\tdirection <%g, %g, %g>\n", direction[0], direction[1], direction[2]);
	fprintf(out, "\tangle %g\n", f2*camera.fov());
	fprintf(out, "}\n\n");

	fprintf(out, "\n# declare cpy_camera_pos = <%g, %g, %g>;\n", eye[0], eye[1], eye[2]);
	// optional fog
	fprintf(out, "# if (DRAW_FOG = 1)\n");
	fprintf(out, "fog {\n\tfog_type 2\n\tup vnormalize(cpy_camera_pos)\n\tcolor rgbt<1,1,1,0.3>\n\tdistance 1e-5\n\tfog_alt 3e-3\n\tfog_offset %g\n}\n", scene.get_box_size()[2]*0.8);
	fprintf(out, "# end\n\n");

	// lights
	vec3 ambient_color = lights.ambient;
	fprintf(out, "\n// LIGHTS\n\n");
	fprintf(out, "# declare ambient_lum = %f;\n", ambient_multiplier);
	fprintf(out, "global_settings {\n\
	ambient_light rgb ambient_lum * <%g, %g, %g>\n\
	max_trace_level 15\n\
	assumed_gamma 2.2\n\
}\n", ambient_color[0], ambient_color[1], ambient_color[2]);

	auto point_at = scene.get_centre_location();
	for(int i = 0; i < 8; i++) {
		Light l = lights.get_light(i);
		if(l.enabled) {
			// we put 10000 here to mimic directional light sources since povray requires a position even for parallel lights
			vec4 pos = glm::vec4(point_at + 10000.f * l.direction, 1.f);
			fprintf(out, "light_source\n{");
			fprintf(out, "\t<%g, %g, %g>,\n", pos[0], pos[1], pos[2]);
			fprintf(out, "\trgb 1.0\n");
			fprintf(out, "\tparallel\n");
			fprintf(out, "\tpoint_at <%g, %g, %g>\n}\n\n", point_at[0], point_at[1], point_at[2]);
		}
	}

	_drawer.init();
	_drawer.set_out_file(out);
	_drawer.visit(scene);

	fclose(out);

	Logging::log_info("done\n");
}
