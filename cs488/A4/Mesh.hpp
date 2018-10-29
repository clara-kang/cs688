// Fall 2018

#pragma once

#include <vector>
#include <iosfwd>
#include <string>

#include <glm/glm.hpp>

#include "Primitive.hpp"

struct Triangle
{
	size_t v1;
	size_t v2;
	size_t v3;

	Triangle( size_t pv1, size_t pv2, size_t pv3 )
		: v1( pv1 )
		, v2( pv2 )
		, v3( pv3 )
	{}
};

// A polygonal mesh.
class Mesh : public Primitive {
public:
  Mesh( const std::string& fname );
	bool intersect(vec3 eye, vec3 ray_dir, double *t, vec3 *n);

private:
	std::vector<glm::vec3> m_vertices;
	std::vector<Triangle> m_faces;
	NonhierSphere m_bounding_sphere;

    friend std::ostream& operator<<(std::ostream& out, const Mesh& mesh);

	bool intersectTriangle(vec3 eye, vec3 ray_dir, double *t, Triangle triangle, vec3 *n);
	void computeBoundingSphere();
};
