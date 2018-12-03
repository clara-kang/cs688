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

bool CurveGroup::intersect(vec3 eye, vec3 ray_dir, Intersection *isect, bool shadow_ray){
	// cout << "mesh intersect" << endl;
	Intersection tmp_isect = Intersection();
	bool intersect = false;
	if (m_bounding_sphere.intersect(eye, ray_dir, &tmp_isect, shadow_ray)) {
		if (render_bb) {
			isect->t = tmp_isect.t;
			isect->normal = tmp_isect.normal;
			return true;
		} else {
			isect->t == HUGE_VAL;
			for (Curve c : m_curves) {
				mat4 to_rays = glm::lookAt(eye, eye + ray_dir, vec3(0.68,0.2,3.0));
				vec3 cp_rays[4];
				for (int i = 0; i < 4; i++ ){
					cp_rays[i] = vec3(to_rays*vec4(c.cp[i],1.0));
				}
				if (intersectCurve(eye, ray_dir, Curve(cp_rays[0],cp_rays[1],cp_rays[2],cp_rays[3]), &tmp_isect, 0.0, 1.0, max_depth, shadow_ray)) {
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
	double u0, double u1, int depth, bool shadow_ray){

	vec3 *cp_rays = c.cp;
	double min_x = std::fmin(std::fmin(cp_rays[0][0], cp_rays[1][0]), std::fmin(cp_rays[2][0], cp_rays[3][0]))-wStart;
	double min_y = std::fmin(std::fmin(cp_rays[0][1], cp_rays[1][1]), std::fmin(cp_rays[2][1], cp_rays[3][1]))-wStart;
	double max_x = std::fmax(std::fmax(cp_rays[0][0], cp_rays[1][0]), std::fmax(cp_rays[2][0], cp_rays[3][0]))+wStart;
	double max_y = std::fmax(std::fmax(cp_rays[0][1], cp_rays[1][1]), std::fmax(cp_rays[2][1], cp_rays[3][1]))+wStart;
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
		double mid = u0+(u1-u0)/2.0;
		bool intersect_c1 = intersectCurve(eye, ray_dir, c1, &isect1, u0, mid, depth-1, shadow_ray);
		bool intersect_c2 = intersectCurve(eye, ray_dir, c2, &isect2, mid, u1, depth-1, shadow_ray);
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
		vec3 segment = c.cp[3] - c.cp[0];
		// get distance between ray and segment
		vec3 ray2seg = glm::normalize(glm::cross(segment,vec3(0,0,-1)));
		double D = -glm::dot(ray2seg, c.cp[0]);
		double dist = D/glm::length(ray2seg);
		if (abs(dist) < wStart) {
			double w = -(ray2seg[0] * dist + c.cp[0][0])/segment[0]; // on segment
			double t = -(ray2seg[2] * dist + c.cp[0][2] + w * segment[2]); // on ray
			if (t < 0) {
				return false;
			}
			//calculate the actual width
			double u = u0 + clamp(w, 0.0, 1.0)*(u1-u0);
			double width = wStart + u * (wEnd - wStart);
			if (abs(dist) > width) {
				return false;
			}
			vec3 intersection = vec3(0,0,-glm::length(ray_dir)) * t;
			if (w < 0) {
				// test against curve start plane
				vec3 curve_tan = c.cp[1] - c.cp[0];
				double D = - glm::dot(c.cp[0], curve_tan);
				double side = glm::dot(curve_tan, intersection) + D;
				// cout << "side1" << side << endl;
				if ( side < 0) {
					return false;
				}
			} else if (w > 1.0) {
				// test against curve end plane
				vec3 curve_tan = c.cp[2] - c.cp[3];
				double D = - glm::dot(c.cp[3], curve_tan);
				double side = glm::dot(curve_tan, intersection) + D;
				// cout << "side2" << side << endl;
				if ( side < 0) {
					return false;
				}
			}
			isect->t = t;
			if (!shadow_ray) {
				vec3 rayntan = glm::cross(vec3(0,0,-1), segment);
				vec3 normal = glm::normalize(glm::cross(rayntan, segment));
				isect->normal = normal;
			}
		}

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
