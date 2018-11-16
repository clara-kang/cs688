// Fall 2018

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <glm/ext.hpp>
#include <glm/gtx/string_cast.hpp>

using namespace std;
using namespace glm;

// #include "cs488-framework/ObjFileDecoder.hpp"
#include "Mesh.hpp"

static const int MIN_FACES_WITHOUT_BOUNDING_VOL = 6;
static bool render_bb = false;

Mesh::Mesh( const std::string& fname )
	: m_vertices()
	, m_faces()
	, m_bounding_sphere(NonhierSphere(vec3(0.0,0.0,0.0), 1.0))
{
	std::string code;
	double vx, vy, vz;
	double tx, ty;
	size_t uv[3];
	size_t s[3];
	std::string f[3];
	std::string delim = "/";

	std::ifstream ifs( fname.c_str() );
	std::string line, model_name;
	while (getline(ifs, line)) {
		stringstream(line) >> code;
		if (code == "o") {
			int use_uv;
			stringstream(line) >> code >> model_name >> use_uv;
			// if (use_uv == 1) {
			// 	cout << "use uv" << endl;;
			// } else {
			// 	cout << "not using uv" << endl;
			// }
			has_uv = use_uv;
			break;
		}
	}
	while( ifs >> code ) {
		if( code == "v" ) {
			ifs >> vx >> vy >> vz;
			m_vertices.push_back( glm::vec3( vx, vy, vz ) );
		} else if( code == "f" ) {
			if (has_uv) {
				ifs >> f[0] >> f[1] >> f[2];
				int pos;
				for (int i = 0; i < 3; i++) {
					pos = f[i].find(delim);
					stringstream(f[i].substr(0, pos)) >> s[i];
					stringstream(f[i].substr(pos+1, strlen(f[i].c_str()))) >> uv[i];
				}
				// cout << f[0] << " " << f[1] << " " << f[2] << endl;
				// cout << "s[0]: " << s[0] << ", s[1]: " << s[1] << ", s[2]: " << s[2] << endl;
				// cout << "uv[0]: " << uv[0] << ", uv[1]: " << uv[1] << ", uv[2]: " << uv[2] << endl;
				Triangle t = Triangle( s[0] - 1, s[1] - 1, s[2] - 1 );
				t.SetUVs(uv[0]-1, uv[1]-1, uv[2]-1);
				m_faces.push_back( t );
			} else {
				ifs >> s[0] >> s[1] >> s[2];
				Triangle t = Triangle( s[0] - 1, s[1] - 1, s[2] - 1 );
				m_faces.push_back( t );
			}
		} else if ( code == "vt") {
			ifs >> tx >> ty;
			m_uvs.push_back( glm::vec2 (tx, ty));
		}
	}
	// cout << "num of faces: " << m_faces.size() << endl;
	// cout << "num of verts: " << m_vertices.size() << endl;
	// cout << "num of uvs: "  << m_uvs.size() << endl;
	if (m_faces.size() > MIN_FACES_WITHOUT_BOUNDING_VOL) {
		has_bounding_volume = true;
		computeBoundingSphere();
	} else {
		has_bounding_volume = false;
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

bool Mesh::intersect(vec3 eye, vec3 ray_dir, double *t, vec3 *n, vec3 *tg, vec2 *uv){
	// cout << "mesh intersect" << endl;
	double t_test;
	vec3 normal;
	vec3 tangent;
	vec2 uv_coord;
	*t = HUGE_VAL;
	if (!has_bounding_volume || m_bounding_sphere.intersect(eye, ray_dir, &t_test, &normal, &tangent, &uv_coord)) {
		if (has_bounding_volume && render_bb) {
			*t = t_test;
			*n = normal;
		} else {
			for (Triangle triangle: m_faces) {
				if (intersectTriangle(eye, ray_dir, &t_test, triangle, &normal, &tangent, &uv_coord)) {
					if (t_test < *t) {
						*t = t_test;
						*n = normal;
						*tg = tangent;
						*uv = uv_coord;
					}
				}
			}
		}
		if (*t < HUGE_VAL) {
			return true;
		}
	}
	return false;
}

bool Mesh::intersectTriangle(vec3 eye, vec3 ray_dir, double *t, Triangle triangle, vec3 *n, vec3 *tg, vec2* uv) {
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
	if (*t > 0) {
		*n = glm::normalize(glm::cross(col1, col2));
		// cout << "v1: " << triangle.v1 << ",v2: " << triangle.v2 << ", v3: " << triangle.v3 << endl;
		// cout << "uv1: " << triangle.uv1 << ",uv2: " << triangle.uv2 << ", uv3: " << triangle.uv3 << endl;
		// cout << endl;
		if (has_uv) {
			*uv = m_uvs.at(triangle.uv1) * (1-beta-gamma) + m_uvs.at(triangle.uv2) * beta + m_uvs.at(triangle.uv3) * gamma;
			vec2 deltaUV1 =  m_uvs.at(triangle.uv2) - m_uvs.at(triangle.uv1);
			vec2 deltaUV2 = m_uvs.at(triangle.uv3) - m_uvs.at(triangle.uv1);
			double r = 1.0/(deltaUV1[0]*deltaUV2[1] - deltaUV1[1]*deltaUV2[0]);
			*tg = (-col1 * deltaUV2[1] + col2 * deltaUV2[0]) * r;
		}
		return true;
	}
	return false;
}

void Mesh::computeBoundingSphere() {
	vec3 center = vec3(0.0,0.0,0.0);
	for (vec3 vertex : m_vertices) {
		center += vertex;
	}
	center = center / m_vertices.size();
	double radius = 0.0;
	for (vec3 vertex : m_vertices) {
		double dist = glm::length(center - vertex);
		radius = std::fmax(radius, dist);
	}
	m_bounding_sphere.m_pos = center;
	m_bounding_sphere.m_radius = radius;
}
