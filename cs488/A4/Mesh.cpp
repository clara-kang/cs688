// Fall 2018

#include <iostream>
#include <fstream>

#include <glm/ext.hpp>
#include <glm/gtx/string_cast.hpp>

using namespace std;
using namespace glm;

// #include "cs488-framework/ObjFileDecoder.hpp"
#include "Mesh.hpp"

Mesh::Mesh( const std::string& fname )
	: m_vertices()
	, m_faces()
{
	std::string code;
	double vx, vy, vz;
	size_t s1, s2, s3;

	std::ifstream ifs( fname.c_str() );
	while( ifs >> code ) {
		if( code == "v" ) {
			ifs >> vx >> vy >> vz;
			m_vertices.push_back( glm::vec3( vx, vy, vz ) );
		} else if( code == "f" ) {
			ifs >> s1 >> s2 >> s3;
			m_faces.push_back( Triangle( s1 - 1, s2 - 1, s3 - 1 ) );
		}
	}
}

std::ostream& operator<<(std::ostream& out, const Mesh& mesh)
{
  out << "mesh {";
  /*

  for( size_t idx = 0; idx < mesh.m_verts.size(); ++idx ) {
  	const MeshVertex& v = mesh.m_verts[idx];
  	out << glm::to_string( v.m_position );
	if( mesh.m_have_norm ) {
  	  out << " / " << glm::to_string( v.m_normal );
	}
	if( mesh.m_have_uv ) {
  	  out << " / " << glm::to_string( v.m_uv );
	}
  }

*/
  out << "}";
  return out;
}

bool Mesh::intersect(vec3 eye, vec3 ray_dir, double *t, vec3 *n){
	double t_test;
	vec3 normal;
	*t = HUGE_VAL;
	for (Triangle triangle: m_faces) {
		if (intersectTriangle(eye, ray_dir, &t_test, triangle, &normal)) {
			if (t_test < *t) {
				*t = t_test;
				*n = normal;
			}
		}
	}
	if (*t < HUGE_VAL) {
		return true;
	}
	return false;
}

bool Mesh::intersectTriangle(vec3 eye, vec3 ray_dir, double *t, Triangle triangle, vec3 *n) {
	vec3 col1 = m_vertices.at(triangle.v1) - m_vertices.at(triangle.v2);
	vec3 col2 = m_vertices.at(triangle.v1) - m_vertices.at(triangle.v3);
	vec3 X = m_vertices.at(triangle.v1) - eye;
	double detA = glm::determinant(mat3(col1, col2, ray_dir));
	double detBeta = glm::determinant(mat3(X, col2, ray_dir));
	double beta = detBeta / detA;
	if (beta < 0 || beta > 1) {
		return false;
	}
	double detGamma = glm::determinant(mat3(col1, X, ray_dir));
	double gamma = detGamma / detA;
	if (gamma < 0 || gamma > 1-beta) {
		return false;
	}
	double detT = glm::determinant(mat3(col1, col2, X));
	*t = detT / detA;
	*n = glm::normalize(glm::cross(col1, col2));
	return true;
}
