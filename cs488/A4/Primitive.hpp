// Fall 2018

#pragma once

#include <glm/glm.hpp>
using namespace glm;

struct Intersection {
	double t;
	vec3 normal;
	vec2 uv;
	vec3 tangent;
	Intersection(double pt, vec3 pnormal, vec2 puv, vec3 ptangent)
	: t(pt), normal(pnormal), uv(puv), tangent(ptangent){
	}
	Intersection()
	: t(HUGE_VAL), normal(vec3(1.0)), uv(vec2(1.0)), tangent(vec3(1.0)){
	}
};

class Primitive {
public:
  virtual ~Primitive();
  virtual  bool intersect(vec3 eye, vec3 ray_dir, Intersection *isect, bool shadow_ray) {return true;}
  bool has_uv = false;
};

class Sphere : public Primitive {
public:
  virtual ~Sphere();
  bool intersect(vec3 eye, vec3 ray_dir, Intersection *isect, bool shadow_ray);
private:
  glm::vec3 m_pos;
  double m_radius;
};

class Cube : public Primitive {
public:
  virtual ~Cube();
  bool intersect(vec3 eye, vec3 ray_dir, Intersection *isect, bool shadow_ray);
};

class NonhierSphere : public Primitive {
public:
  NonhierSphere(const glm::vec3& pos, double radius)
    : m_pos(pos), m_radius(radius)
  {
  }
  virtual ~NonhierSphere();
  bool intersect(vec3 eye, vec3 ray_dir, Intersection *isect, bool shadow_ray);

// private:
  glm::vec3 m_pos;
  double m_radius;
};

class NonhierBox : public Primitive {
public:
  NonhierBox(const glm::vec3& pos, double size)
    : m_pos(pos), m_size(size)
  {
    m_pos += vec3(0.0,0.0,0.0);
  }
  bool intersect(vec3 eye, vec3 ray_dir, Intersection *isect, bool shadow_ray);
  virtual ~NonhierBox();

private:
  bool isWithin(double posx, double posy, double xmin, double ymin, double xmax, double ymax);
  glm::vec3 m_pos;
  double m_size;
};
