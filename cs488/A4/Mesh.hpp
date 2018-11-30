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

	size_t uv1;
	size_t uv2;
	size_t uv3;

	size_t vn1;
	size_t vn2;
	size_t vn3;

	Triangle( size_t pv1, size_t pv2, size_t pv3)
		: v1( pv1 )
		, v2( pv2 )
		, v3( pv3 )
	{}

	void SetUVs(size_t puv1, size_t puv2, size_t puv3) {
		uv1 = puv1;
		uv2 = puv2;
		uv3 = puv3;
	}

	void SetVNs(size_t pvn1, size_t pvn2, size_t pvn3) {
		vn1 = pvn1;
		vn2 = pvn2;
		vn3 = pvn3;
	}
};

// A polygonal mesh.
class Mesh : public Primitive {
public:
  Mesh( const std::string& fname );
	bool intersect(vec3 eye, vec3 ray_dir, Intersection *isect);

private:
	std::vector<glm::vec3> m_vertices;
	std::vector<Triangle> m_faces;
	std::vector<glm::vec2> m_uvs;
	std::vector<glm::vec3> m_vnormals;
	NonhierSphere m_bounding_sphere;

    friend std::ostream& operator<<(std::ostream& out, const Mesh& mesh);
	bool has_bounding_volume;
	bool has_gouroud;
	bool intersectTriangle(vec3 eye, vec3 ray_dir, Triangle triangle, Intersection *isect);
	void computeBoundingSphere();
};
