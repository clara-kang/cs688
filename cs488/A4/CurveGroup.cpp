// Fall 2018

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

using namespace std;
using namespace glm;

#include "CurveGroup.hpp"
#include "polyroots.hpp"

static bool render_bb = false;
static const int VERTICES_IN_LOOP = 5;
CurveGroup::CurveGroup( const std::string& fname )
	: m_vertices()
	, m_curves()
	, m_bounding_sphere(NonhierSphere(vec3(0.0,0.0,0.0), 1.0))
{
	std::string code;
	double vx, vy, vz;

	std::ifstream ifs( fname.c_str() );

	while( ifs >> code ) {
		if( code == "v" ) {
			ifs >> vx >> vy >> vz;
			m_vertices.push_back( glm::vec3( vx, vy, vz ) );
		}
	}
	int num_vertices = m_vertices.size();
	for (int i = 0; i < num_vertices / VERTICES_IN_LOOP; i++) {
		vec3 p1 = m_vertices.at(i*VERTICES_IN_LOOP + 0);
		vec3 p2 = m_vertices.at(i*VERTICES_IN_LOOP + 1);
		vec3 p3 = m_vertices.at(i*VERTICES_IN_LOOP + 2);
		vec3 p4 = m_vertices.at(i*VERTICES_IN_LOOP + 4);
		m_curves.push_back(Curve(p1, p2, p3, p4));

	}
	cout << "# of curves: " << m_curves.size() << endl;
	computeBoundingSphere();

}

bool CurveGroup::intersect(vec3 eye, vec3 ray_dir, Intersection *isect){
	// cout << "mesh intersect" << endl;
	Intersection tmp_isect = Intersection();
	bool intersect = false;
	if (m_bounding_sphere.intersect(eye, ray_dir, &tmp_isect)) {
		if (render_bb) {
			isect->t = tmp_isect.t;
			isect->normal = tmp_isect.normal;
		} else {
			isect->t == HUGE_VAL;
			for (Curve c : m_curves) {
				mat4 to_rays = glm::lookAt(eye, eye + ray_dir, vec3(0.68,0.2,3.0));
				vec3 cp_rays[4];
				for (int i = 0; i < 4; i++ ){
					cp_rays[i] = vec3(to_rays*vec4(c.cp[i],1.0));
				}
				if (intersectCurve(eye, ray_dir, Curve(cp_rays[0],cp_rays[1],cp_rays[2],cp_rays[3]), &tmp_isect, 0.0, 1.0, max_depth)) {
					intersect = true;
					if (tmp_isect.t < isect->t) {
						*isect = tmp_isect;
					}
				}
			}
		}
		if (intersect) {
			return true;
		}
	}
	return false;
}

bool CurveGroup::intersectCurve(vec3 eye, vec3 ray_dir, Curve c, Intersection *isect,
	double u0, double u1, int depth){

	vec3 *cp_rays = c.cp;
	double min_x = std::fmin(std::fmin(cp_rays[0][0], cp_rays[1][0]), std::fmin(cp_rays[2][0], cp_rays[3][0])) - wStart/2.0;
	double min_y = std::fmin(std::fmin(cp_rays[0][1], cp_rays[1][1]), std::fmin(cp_rays[2][1], cp_rays[3][1])) + wStart/2.0;
	double max_x = std::fmax(std::fmax(cp_rays[0][0], cp_rays[1][0]), std::fmax(cp_rays[2][0], cp_rays[3][0])) - wStart/2.0;
	double max_y = std::fmax(std::fmax(cp_rays[0][1], cp_rays[1][1]), std::fmax(cp_rays[2][1], cp_rays[3][1])) + wStart/2.0;

	// if within bounding box
	if (!(min_x <= 0 && max_x >=0 && min_y <=0 && max_y >= 0)) {
		return false;
	}
	// blossom
	if (depth > 0) {
		vec3 cpSplit[7];
		subdivideBezier(c.cp, cpSplit);
		Curve c1 = Curve(cpSplit[0], cpSplit[1], cpSplit[2], cpSplit[3]);
		Curve c2 = Curve(cpSplit[3], cpSplit[4], cpSplit[5], cpSplit[6]);
		Intersection isect1 = Intersection();
		Intersection isect2 = Intersection();
		bool intersect_c1 = intersectCurve(eye, ray_dir, c1, &isect1, u0, (u1-u0)/2.0, depth-1);
		bool intersect_c2 = intersectCurve(eye, ray_dir, c2, &isect2, (u1-u0)/2.0, u1, depth-1);
		if (intersect_c1 || intersect_c2) {
			if (isect1.t < isect2.t) {
				*isect = isect1;
			} else {
				*isect = isect2;
			}
			return true;
		} else {
			return false;
		}
	}
	if (depth == 0) {
		// approximate with line segment
		vec3 line = c.cp[3] - c.cp[0];
		mat4 to_lines = glm::lookAt(c.cp[0], c.cp[0]+line, vec3(0.68,0.2,3.0));
		// vec3 eye_lines = vec3(to_lines * vec4(eye, 1.0));
		// vec3 ray_lines = vec3(to_lines * vec4(ray_dir, 0.0));
		vec3 eye_lines = vec3(to_lines * vec4(0.0,0.0,0.0, 1.0));
		vec3 ray_lines = vec3(to_lines * vec4(0.0,0.0,-glm::length(ray_dir), 0.0));
		vec3 trans_line = vec3(to_lines * vec4(line, 0.0));

		// calculate intersection with line segment
		double width_u0 = wStart + (wEnd-wStart)*u0;
		double width_u1 = wStart + (wEnd-wStart)*u1;
		double length = glm::length(line);
		double radius_slope = (width_u1 - width_u0)/length;
		double A = pow(ray_lines[0],2.0) + pow(ray_lines[1],2.0) - pow(radius_slope, 2.0);
		double B = 2.0*eye_lines[0]*ray_lines[0] + 2.0*eye_lines[1]*ray_lines[1] -
			2.0*pow(radius_slope, 2.0)*eye_lines[2]*ray_lines[2] - 2.0*width_u0*radius_slope;
		double C = pow(eye_lines[0],2.0) + pow(eye_lines[1],2.0) - pow(width_u0,2.0) -
			pow(radius_slope,2.0)*pow(eye_lines[2],2.0) - 2.0*width_u0*radius_slope*eye_lines[2];
		double roots[2];
		size_t num_roots = quadraticRoots(A,B,C,roots);
		if (num_roots == 1 && roots[0] > 0) {
	    isect->t = roots[0];
	  } else if (num_roots == 2) {
	    double s_root = std::fmin(roots[0],roots[1]);
	    double b_root = std::fmax(roots[0],roots[1]);
	    if (b_root < 0) {
	      return false;
	    } else if (s_root < 0 && b_root >= 0){
	      isect->t = b_root;
	    } else if (s_root >= 0) {
	      isect->t = s_root;
	    }
	  } else {
	    return false;
	  }
		// check z in range
		vec3 isect_lines = eye_lines + isect->t * ray_lines;
		if (isect_lines[2] <= length) {
			// calculate normal
			vec3 normal_lines = vec3(isect_lines[0], isect_lines[1], 0.0);
			isect->normal = vec3(glm::inverse(to_lines) * vec4(normal_lines,0.0));
			cout << "true" << endl;
			return true;
		}
		return false;
	}
}

void CurveGroup::subdivideBezier(const vec3 cp[4], vec3 cpSplit[7]) {
	cpSplit[0] = cp[0];
	cpSplit[1] = (cp[0] + cp[1]) / 2.0;
	cpSplit[2] = (cp[0] + 2.0 * cp[1] + cp[2]) / 4.0;
	cpSplit[3] = (cp[0] + 3.0 * cp[1] + 3.0 * cp[2] + cp[3]) / 8.0;
	cpSplit[4] = (cp[1] + 2 * cp[2] + cp[3]) / 4.0;
	cpSplit[5] = (cp[2] + cp[3]) / 2.0;
	cpSplit[6] = cp[3];
}

void CurveGroup::computeBoundingSphere() {
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
