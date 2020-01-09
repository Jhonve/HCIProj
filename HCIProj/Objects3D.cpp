#include "Objects3D.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <qfileinfo.h>

Objects3D::Objects3D()
{

}

Objects3D::Objects3D(const char* file_name)
{
	initializeData(file_name);
}

Objects3D::~Objects3D()
{
	delete[] m_vertices_data;
	delete[] m_face_indices;

	m_positions.clear();
	m_normals.clear();
	m_vertex_indices.clear();
	m_face_vertices.clear();
	m_face_normals.clear();
}

void Objects3D::calculateFaceNormal()
{
	for (int i_face = 0; i_face < m_num_faces; i_face++)
	{
		glm::fvec3 position_0(m_positions[m_vertex_indices[i_face][0]]);
		glm::fvec3 position_1(m_positions[m_vertex_indices[i_face][1]]);
		glm::fvec3 position_2(m_positions[m_vertex_indices[i_face][2]]);

		glm::fvec3 vector_a(0.f, 0.f, 0.f);
		glm::fvec3 vector_b(0.f, 0.f, 0.f);

		vector_a = position_2 - position_0;
		vector_b = position_0 - position_1;

		glm::fvec3 vector_normal(0.f, 0.f, 0.f);
		vector_normal[0] = vector_a[1] * vector_b[2] - vector_a[2] * vector_b[1];
		vector_normal[1] = vector_a[2] * vector_b[0] - vector_a[0] * vector_b[2];
		vector_normal[2] = vector_a[0] * vector_b[1] - vector_a[1] * vector_b[0];

		glm::fvec3 normalized_vec = glm::normalize(vector_normal);

		m_face_normals.push_back(normalized_vec);
	}
}

void Objects3D::calculateVertexNormal()
{
	for (int i_vert = 0; i_vert < m_num_vertices; i_vert++)
	{
		std::vector<int>::iterator iter_neighbor;
		int num_neighbor = 0;
		glm::fvec3 vertex_normal(0.f, 0.f, 0.f);

		for (iter_neighbor = m_face_indices[i_vert].begin(); iter_neighbor != m_face_indices[i_vert].end(); iter_neighbor++)
		{
			vertex_normal += m_face_normals[m_face_indices[i_vert][num_neighbor]];
			num_neighbor++;
		}
		if (vertex_normal[0] == 0 && vertex_normal[1] == 0 && vertex_normal[2] == 0)
		{
			vertex_normal = glm::fvec3(0.f, 0.f, 0.f);
		}
		else
		{
			vertex_normal = glm::normalize(vertex_normal);
		}

		if (num_neighbor == 0)
		{
			std::cout << "_________________" << std::endl;
			std::cout << "Exsist un-used vertex " << i_vert << " !" << std::endl;
			std::cout << "_________________" << std::endl;
			vertex_normal = glm::fvec3(0.f, 0.f, 0.f);
		}

		m_normals.push_back(vertex_normal);
	}
}

void Objects3D::readFileOFF(const char* file_name)
{
	std::ifstream file_in(file_name);
	std::string string_line;
	// int i = 0, j = 0;

	int num_point, num_face, num_all_line = 0;
	int line_count = 0;
	float p_x, p_y, p_z = 0.f;	// postion of vertex
	int vi_n, vi_0, vi_1, vi_2 = 0;	//num of vertexs in this face, vertex index of face

	glm::fvec3 max_coord = glm::fvec3(-100.f, -100.f, -100.f);
	glm::fvec3 min_coord = glm::fvec3(100.f, 100.f, 100.f);

	while (std::getline(file_in, string_line))
	{
		if (line_count == 0)
		{
			line_count++;
		}
		else if (line_count == 1)
		{
			std::istringstream string_in(string_line);
			string_in >> num_point >> num_face >> num_all_line;
			string_in.clear();

			m_face_indices = new std::vector<int>[num_point];

			line_count++;
		}
		else
		{
			if (line_count < num_point + 2)
			{
				std::istringstream string_in(string_line);
				string_in >> p_x >> p_y >> p_z;
				glm::fvec3 point_pos = glm::fvec3(p_x, p_y, p_z);
				m_positions.push_back(point_pos);

				m_center += point_pos;
				for (int i_m = 0; i_m < 3; i_m++)
				{
					if (max_coord[i_m] < point_pos[i_m])
					{
						max_coord[i_m] = point_pos[i_m];
					}
					if (min_coord[i_m] > point_pos[i_m])
					{
						min_coord[i_m] = point_pos[i_m];
					}
				}

				string_in.clear();
				line_count++;
			}
			else if (line_count >= num_point + 2 && line_count < num_point + num_face + 2)
			{
				std::istringstream string_in(string_line);
				string_in >> vi_n >> vi_0 >> vi_1 >> vi_2;

				if (vi_n != 3)
				{
					std::cout << "_______________" << std::endl;
					std::cout << "Exsist un-triangle mash!" << std::endl;
					std::cout << "_______________" << std::endl;
				}

				m_vertex_indices.push_back(glm::ivec3(vi_0, vi_1, vi_2));
				m_face_vertices.push_back(glm::fvec3(m_positions[vi_0][0], m_positions[vi_0][1], m_positions[vi_0][2]));
				m_face_vertices.push_back(glm::fvec3(m_positions[vi_1][0], m_positions[vi_1][1], m_positions[vi_1][2]));
				m_face_vertices.push_back(glm::fvec3(m_positions[vi_2][0], m_positions[vi_2][1], m_positions[vi_2][2]));
				// add face indices to each vertex.
				m_face_indices[vi_0].push_back(line_count - num_point - 2);
				m_face_indices[vi_1].push_back(line_count - num_point - 2);
				m_face_indices[vi_2].push_back(line_count - num_point - 2);

				string_in.clear();
				line_count++;
			}
		}
	}
	if (line_count == 0)
	{
		// No such file
		m_num_vertices = 0;
		return;
	}

	m_num_vertices = num_point;
	m_num_faces = num_face;

	m_center = m_center / (float)num_point;
	for (int i_c = 0; i_c < 3; i_c++)
	{
		if (m_max_coord < max_coord[i_c] - min_coord[i_c])
		{
			m_max_coord = max_coord[i_c] - min_coord[i_c];
		}
	}
	m_max_coord /= 2.f;

	calculateFaceNormal();
	calculateVertexNormal();

	file_in.close();
}

void Objects3D::readFileOBJ(const char* file_name)
{

}

void Objects3D::initializeData(const char* file_name)
{
	QString q_file_name = QFileInfo(file_name).fileName();

	if (q_file_name.contains("off"))
	{
		readFileOFF(file_name);
		if (m_num_vertices == 0)
		{
			return;
		}
		std::cout << "Reading \'.off\' successful ! Number of points is " << m_num_vertices << "  And number of faces is " << m_num_faces << std::endl;
	}
	else if (q_file_name.contains("obj"))
	{
		readFileOBJ(file_name);
	}
	else
	{
		std::cout << "Wrong file type!" << std::endl;
		return;
	}

	m_vertices_data = new GLfloat[m_num_faces * 3 * 9];
	for (int i_num = 0; i_num < m_num_faces * 3; i_num++)
	{
		for (int i_data = 0; i_data < 9; i_data++)
		{
			if (i_data < 3)
			{
				m_vertices_data[i_num * 9 + i_data] = (m_face_vertices[i_num][i_data] - m_center[i_data]) * (0.5f / m_max_coord);
			}
			else if(i_data >=3 && i_data < 6)
			{
				// point normal
				m_vertices_data[i_num * 9 + i_data] = m_normals[m_vertex_indices[int(i_num / 3)][i_num % 3]][i_data - 3];
				// face normal
				// m_vertices_data[i_num * 9 + i_data] = m_face_normals[int(i_num / 3)][i_data - 3];
			}
			else
			{
				// use point normal as normal
				m_vertices_data[i_num * 9 + i_data] = (m_normals[m_vertex_indices[int(i_num / 3)][i_num % 3]][i_data - 6] + 1.f) / 2.f;

				//if (i_data % 3 == 0) // R
				//{
				//	m_vertices_data[i_num * 9 + i_data] = 0.5f;
				//}
				//else if (i_data % 3 == 1) // G
				//{
				//	m_vertices_data[i_num * 9 + i_data] = 0.5f;
				//}
				//else // B
				//{
				//	m_vertices_data[i_num * 9 + i_data] = 1.0f;
				//}
			}
		}
	}
}