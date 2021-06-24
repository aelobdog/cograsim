/*
 * OGLIcosahedron.cpp
 *
 *  Created on: 03/nov/2013
 *      Author: Flavio
 */

#include "OGLIcosahedron.h"
#include "../../Utils/MathHelper.h"

std::shared_ptr<Mesh> OGLIcosahedron::get_mesh() {
	// we assume that the icosahedron is inscribed within a sphere of radius 0.5
	
	// rename the member vectors to shorter names
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	// vertexes and normal of a dodecahedron
	std::vector<GLfloat> myvert(3 * 12); // 12 vertices
/*
 +0.0000000000  -0.5257311121  -0.8506508084
 -0.8506508084  +0.0000000000  -0.5257311121
 -0.5257311121  -0.8506508084  +0.0000000000
 +0.0000000000  -0.5257311121  +0.8506508084
 +0.8506508084  +0.0000000000  -0.5257311121
 -0.5257311121  +0.8506508084  +0.0000000000
 +0.0000000000  +0.5257311121  -0.8506508084
 -0.8506508084  +0.0000000000  +0.5257311121
 +0.5257311121  -0.8506508084  +0.0000000000
 +0.0000000000  +0.5257311121  +0.8506508084
 +0.8506508084  +0.0000000000  +0.5257311121
 +0.5257311121  +0.8506508084  +0.0000000000
*/
	GLfloat a = 0.525731112119;
	GLfloat b = 0.850650808352;

	myvert[ 0] =  0; myvert[ 1] =  a; myvert[ 2] =  b;
	myvert[ 3] =  0; myvert[ 4] =  a; myvert[ 5] = -b;
	myvert[ 6] =  0; myvert[ 7] = -a; myvert[ 8] =  b;
	myvert[ 9] =  0; myvert[10] = -a; myvert[11] = -b;

	myvert[12] =  b; myvert[13] =  0; myvert[14] =  a;
	myvert[15] =  b; myvert[16] =  0; myvert[17] = -a;
	myvert[18] = -b; myvert[19] =  0; myvert[20] =  a;
	myvert[21] = -b; myvert[22] =  0; myvert[23] = -a;
	
	myvert[24] =  a; myvert[25] =  b; myvert[26] =  0;
	myvert[27] =  a; myvert[28] = -b; myvert[29] =  0;
	myvert[30] = -a; myvert[31] =  b; myvert[32] =  0;
	myvert[33] = -a; myvert[34] = -b; myvert[37] =  0;

	for (int i = 0; i < 36; i ++) {
		myvert[i] = myvert[i]/2.;
	}

	// faces, to find out vertexes and normals
	GLushort cntr = 0;
	float edge_length = 0.5 / (sin(2.*M_PI/5.));
	float compare = 1.05*edge_length*edge_length;
	for (int i = 0; i < 12; i ++) {
		for (int j = i+1; j < 12; j ++) {
			for (int k = j+1; k < 12; k ++) { 
				float dx11 = myvert[3*i  ] - myvert[3*j  ];
				float dy11 = myvert[3*i+1] - myvert[3*j+1];
				float dz11 = myvert[3*i+2] - myvert[3*j+2];
				float d11 = dx11*dx11 + dy11*dy11 + dz11*dz11;
				if (d11 > compare) {
					continue;
				}

				float dx12 = myvert[3*i  ] - myvert[3*k  ];
				float dy12 = myvert[3*i+1] - myvert[3*k+1];
				float dz12 = myvert[3*i+2] - myvert[3*k+2];
				float d12 = dx12*dx12 + dy12*dy12 + dz12*dz12;
				if (d12 > compare) {
					continue;
				}

				float dx23 = myvert[3*j  ] - myvert[3*k  ];
				float dy23 = myvert[3*j+1] - myvert[3*k+1];
				float dz23 = myvert[3*j+2] - myvert[3*k+2];
				float d23 = dx23*dx23 + dy23*dy23 + dz23*dz23;
				if (d23 > compare) {
					continue;
				}

				indices.push_back(cntr);
				indices.push_back(cntr + 1);
				indices.push_back(cntr + 2);
				cntr = cntr + 3;

				// find the normal
				glm::vec3 normal;
				normal.x = myvert[3*i]   + myvert[3*j]   + myvert[3*k];
				normal.y = myvert[3*i+1] + myvert[3*j+1] + myvert[3*k+1];
				normal.z = myvert[3*i+2] + myvert[3*j+2] + myvert[3*k+2];
				normal = glm::normalize(normal);

				Vertex v;
				v.position[0] = myvert[3*i];
				v.position[1] = myvert[3*i+1];
				v.position[2] = myvert[3*i+2];
				v.normal = normal;

				vertices.push_back(v);

				v.position[0] = myvert[3*j];
				v.position[1] = myvert[3*j+1];
				v.position[2] = myvert[3*j+2];
				vertices.push_back(v);

				v.position[0] = myvert[3*k];
				v.position[1] = myvert[3*k+1];
				v.position[2] = myvert[3*k+2];
				vertices.push_back(v);
			}
		}
	}
	
	return std::make_shared<Mesh>(vertices, indices);
}
