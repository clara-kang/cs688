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
	if (m_bounding_sphere.intersect(eye, ray_dir, &tmp_isect)) {
		if (render_bb) {
			isect->t = tmp_isect.t;
			isect->normal = tmp_isect.normal;
		} else {
			// for (Curve c : m_curves) {
			// 	if (intersectCurve(eye, ray_dir, &t_test, c, &normal)) {
			// 		if (t_test < *t) {
			// 			*t = t_test;
			// 			// *n = normal;
			// 			*n = -ray_dir;
			// 		}
			// 	}
			// }
		}
		if (isect->t < HUGE_VAL) {
			return true;
		}
	}
	return false;
}

bool isWithin(double x, double y, double xmin, double xmax, double ymin, double ymax) {
	return xmin <= x && x <= xmax && ymin <= y && y <= ymax;
}

int intersectBB(double width, double height, double length, vec3 eye, vec3 ray_dir, double *t_above) {
	double t = HUGE_VAL, t_test;
	vec3 intersection;
	int plane = -1; //1:top, 2:bot, 3:left, 4:right, 5:front, 6:back
	// top plane, y max fixed
	t_test = -(eye[1] - height)/ray_dir[1];
	intersection = eye + t_test * ray_dir;
	if (isWithin(intersection[0], intersection[2], -width/2, width/2, -length, 0.0) && t_test < t && t_test > 0) {
		t = t_test;
		plane = 1;
	}
	// bot plane, y min fixed
	t_test = -eye[1]/ray_dir[1];
	intersection = eye + t_test * ray_dir;
	if (isWithin(intersection[0], intersection[2], -width/2, width/2, -length, 0.0) && t_test < t && t_test > 0) {
		t = t_test;
		plane = 2;
	}
	// left plane, x min fixed
	t_test = -(eye[0] + width/2.0)/ray_dir[0];
	intersection = eye + t_test * ray_dir;
	if (isWithin(intersection[1], intersection[2], 0.0, height, -length, 0.0) && t_test < t && t_test > 0) {
		t = t_test;
		plane = 3;
	}
	// right plane, x max fixed
	t_test = -(eye[0] - width/2.0)/ray_dir[0];
	intersection = eye + t_test * ray_dir;
	if (isWithin(intersection[1], intersection[2], 0.0, height, -length, 0.0) && t_test < t && t_test > 0) {
		t = t_test;
		plane = 4;
	}
	// front plane
	t_test = -eye[2]/ray_dir[2]; // z max fixed
	intersection = eye + t_test * ray_dir;
	if (isWithin(intersection[0], intersection[1], -width/2.0, width/2.0, 0.0, height) && t_test < t && t_test > 0) {
		t = t_test;
		plane = 5;
	}
	// back plane
	t_test = -(eye[2] + length)/ray_dir[2]; // z min fixed
	intersection = eye + t_test * ray_dir;
	if (isWithin(intersection[0], intersection[1], -width/2.0, width/2.0, 0.0, height) && t_test < t && t_test > 0) {
		t = t_test;
		plane = 6;
	}
	*t_above = t;
	// if (plane == 1) {
	// 	cout << "t: " << t << endl;
	// 	cout << "plane : " << plane << endl;
	// }
	return plane;
}


bool CurveGroup::intersectCurve(vec3 eye, vec3 ray_dir, Curve c, Intersection *isect, int depth){
	mat4 to_rays = glm::lookAt(eye, eye + ray_dir, vec3(0.0,1.0,0.0));
	vec3 cp_rays[4];
	for (int i = 0; i < 4; i++ ){
		cp_rays[i] = vec3(to_rays*vec4(c.cp[i],1.0));
	}

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
		return intersectCurve(eye, ray_dir, c1, isect, depth-1);
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
