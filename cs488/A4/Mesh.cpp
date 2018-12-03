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
	double vnx, vny, vnz;
	size_t uv[3];
	size_t s[3];
	size_t vn[3];
	std::string f[3];
	std::string delim = "/";

	std::ifstream ifs( fname.c_str() );
	std::string line, model_name;
	while (getline(ifs, line)) {
		stringstream(line) >> code;
		if (code == "o") {
			int use_uv;
			int use_gouraud;
			stringstream(line) >> code >> model_name >> use_uv >> use_gouraud;
			has_uv = use_uv;
			has_gouroud = use_gouraud;
			break;
		}
	}
	while( ifs >> code ) {
		if( code == "v" ) {
			ifs >> vx >> vy >> vz;
			m_vertices.push_back( glm::vec3( vx, vy, vz ) );
		} else if( code == "f" ) {
			if (has_uv && !has_gouroud) {
				ifs >> f[0] >> f[1] >> f[2];
				int pos;
				for (int i = 0; i < 3; i++) {
					pos = f[i].find(delim);
					stringstream(f[i].substr(0, pos)) >> s[i];
					stringstream(f[i].substr(pos+1, strlen(f[i].c_str()))) >> uv[i];
				}
				Triangle t = Triangle( s[0] - 1, s[1] - 1, s[2] - 1 );
				t.SetUVs(uv[0]-1, uv[1]-1, uv[2]-1);
				m_faces.push_back( t );
			} else if (has_gouroud) {
				ifs >> f[0] >> f[1] >> f[2];
				int pos, pos1;
				for (int i = 0; i < 3; i++) {
					pos = f[i].find(delim);
					stringstream(f[i].substr(0, pos)) >> s[i];
					pos1 = f[i].find(delim, pos+1);
					if (has_uv) {
						stringstream(f[i].substr(pos+1, pos1)) >> uv[i];
					}
					stringstream(f[i].substr(pos1+1, strlen(f[i].c_str()))) >> vn[i];
				}
				Triangle t = Triangle( s[0] - 1, s[1] - 1, s[2] - 1 );
				if (has_uv) {
					t.SetUVs(uv[0]-1, uv[1]-1, uv[2]-1);
				}
				t.SetVNs(vn[0]-1, vn[1]-1, vn[2]-1);
				m_faces.push_back( t );
			} else {
				ifs >> s[0] >> s[1] >> s[2];
				Triangle t = Triangle( s[0] - 1, s[1] - 1, s[2] - 1 );
				m_faces.push_back( t );
			}
		} else if ( code == "vt") {
			ifs >> tx >> ty;
			m_uvs.push_back( glm::vec2 (tx, ty));
		} else if ( code == "vn") {
			ifs >> vnx >> vny >> vnz;
			m_vnormals.push_back( glm::vec3 (vnx, vny, vnz));
		}
	}
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

bool Mesh::intersect(vec3 eye, vec3 ray_dir, Intersection *isect, bool shadow_ray){
	// cout << "mesh intersect" << endl;
	Intersection tmp_isect = Intersection();
	isect->t = HUGE_VAL;
	if (!has_bounding_volume || m_bounding_sphere.intersect(eye, ray_dir, &tmp_isect, shadow_ray)) {
		if (has_bounding_volume && render_bb) {
			isect->t = tmp_isect.t;
			isect->normal = tmp_isect.normal;
		} else {
			for (Triangle triangle: m_faces) {
				if (intersectTriangle(eye, ray_dir, triangle, &tmp_isect, shadow_ray)) {
					if (tmp_isect.t < isect->t) {
						*isect = tmp_isect;
					}
				}
			}
		}
		if (isect->t < HUGE_VAL) {
			return true;
		}
	}
	return false;
}

bool Mesh::intersectTriangle(vec3 eye, vec3 ray_dir, Triangle triangle, Intersection *isect, bool shadow_ray) {
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
	isect->t = detT / detA;
	if (isect->t > 0) {
		if (shadow_ray) {
			return true;
		}
		if (has_uv) {
			vec2 uv = m_uvs.at(triangle.uv1) * (1-beta-gamma) + m_uvs.at(triangle.uv2) * beta + m_uvs.at(triangle.uv3) * gamma;
			uv[0] = fmod(uv[0]+10.0, 1.0);
			uv[1] = fmod(uv[1]+10.0, 1.0);
			if (!(uv[0] >= 0 && uv[0] <=1 && uv[1] >= 0 && uv[1] <=1)) {
				cout << "wrong" << endl;
			}
			isect->uv = uv;
			vec2 deltaUV1 =  m_uvs.at(triangle.uv2) - m_uvs.at(triangle.uv1);
			vec2 deltaUV2 = m_uvs.at(triangle.uv3) - m_uvs.at(triangle.uv1);
			double r = 1.0/(deltaUV1[0]*deltaUV2[1] - deltaUV1[1]*deltaUV2[0]);
			isect->tangent = (-col1 * deltaUV2[1] + col2 * deltaUV2[0]) * r;
		}
		if (has_gouroud) {
			isect->normal = m_vnormals.at(triangle.vn1) * (1-beta-gamma) + m_vnormals.at(triangle.vn2) * beta + m_vnormals.at(triangle.vn3) * gamma;
		} else {
			isect->normal = glm::normalize(glm::cross(col1, col2));
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
