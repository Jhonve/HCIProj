#pragma once

#include <stdlib.h>
#include <vector>
#include "qopenglfunctions_4_5_core.h"
#include <glm.hpp>

class Objects3D
{

public:
	Objects3D();
	Objects3D(const char* file_name);
	~Objects3D();

	int m_num_vertices;
	std::vector<glm::fvec3> m_positions;
	std::vector<glm::fvec3> m_normals;
	GLfloat* m_vertices_data;

	int m_num_faces;
	int m_num_edges;
	std::vector<glm::ivec3> m_vertex_indices;	// vertex index for each face.
	std::vector<glm::fvec3> m_face_vertices; // vertex position for each face.
	std::vector<glm::fvec3> m_face_normals;	 // normal vector for each face.
	std::vector<int>* m_face_indices;	// a 2d vector for each vertex's neighor faces.

private:
	void initializeData(const char* file_name);
	void readFileOBJ(const char* file_name);
	void readFileOFF(const char* file_name);
	void calculateFaceNormal();
	void calculateVertexNormal();

	float m_max_coord = 0.f;
	glm::fvec3 m_center = glm::fvec3(0.f, 0.f, 0.f);
};