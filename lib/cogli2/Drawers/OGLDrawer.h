/*
 * OGLDrawer.h
 *
 *  Created on: 20/ott/2013
 *      Author: lorenzo
 */

#ifndef OGLDRAWER_H_
#define OGLDRAWER_H_

#include "Drawer.h"

#include "../OGL/Shapes/Mesh.h"
#include "../OGL/Shader.h"
#include "../Drawables/Arrow.h"

#include <array>

class OGLDrawer: public Drawer {
	// we want to draw sheres last so we list the entries of this enum accordingly
	enum MeshType {
		CYLINDER,
		CONE,
		ICOSAHEDRON,
		SPHERE
	};

protected:
	int _resolution;
	bool _selection_mode;
	Shape *_curr_shape;
	std::shared_ptr<Shader> _shader;
	glm::vec3 _camera_position;

	std::array<std::shared_ptr<Arrow>, 3> _axes_arrows;

	std::map<MeshType, std::shared_ptr<Mesh>> _meshes;

	void _add_instance_data_to_mesh(MeshType type, const glm::mat4 &model_matrix, const glm::vec4 &color);
	glm::vec4 _get_color(glm::vec4 color);

	std::tuple<glm::mat4, glm::mat4> _get_arrow_model_matrices(Arrow &arrow);

public:
	OGLDrawer();
	virtual ~OGLDrawer();

	void initialise();

	void increase_resolution();

	void decrease_resolution();

	void set_resolution(int res) {
		_resolution = res;
	}

	void set_shader(std::shared_ptr<Shader> shader) {
		_shader = shader;
	}

	void set_camera_position(glm::vec3 pos) {
		_camera_position = pos;
	}

	void toggle_selection_mode();

	void draw();
	void draw_axes_arrows(std::shared_ptr<Shader> shader);

	virtual void visit(Drawable&);
	virtual void visit(Scene&);
	virtual void visit(Cylinder&);
	virtual void visit(Icosahedron&);
	virtual void visit(Sphere&);
	virtual void visit(DNAStrand&);
	virtual void visit(Nucleotide&);
	virtual void visit(DHS&);
	virtual void visit(Arrow&);
	virtual void visit(PatchyParticle&);
	virtual void visit(Triangle&);
	virtual void visit(Group&);
};

#endif /* OGLDRAWER_H_ */
