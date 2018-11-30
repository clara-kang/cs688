// Fall 2018

#pragma once

#include <vector>
#include <iosfwd>
#include <string>

#include <glm/glm.hpp>

#include "Primitive.hpp"

struct Curve
{
	vec3 cp[4];

	Curve( vec3 cp1, vec3 cp2, vec3 cp3, vec3 cp4)
	{
		cp[0] = cp1;
		cp[1] = cp2;
		cp[2] = cp3;
		cp[3] = cp4;
	}

	// Curve ( const size_t & control_points) {
	// 	cp = control_points;
	// }
};

// A polygonal mesh.
class CurveGroup : public Primitive {
public:
  CurveGroup( const std::string& fname );
	bool intersect(vec3 eye, vec3 ray_dir, Intersection *isect);


private:
	std::vector<glm::vec3> m_vertices;
	std::vector<Curve> m_curves;
  double wStart = 0.01; // width at the start of curve
  double wEnd = 0.005; // width at the end of curve
	int max_depth = 3;
	NonhierSphere m_bounding_sphere;

	void computeBoundingSphere();
	bool intersectCurve(vec3 eye, vec3 ray_dir, Curve c, Intersection *isect, double u0, double u1, int depth);
	void subdivideBezier(const vec3 cp[4], vec3 cpSplit[7]);
};
