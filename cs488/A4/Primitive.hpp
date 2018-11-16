// Fall 2018

#pragma once

#include <glm/glm.hpp>
using namespace glm;

class Primitive {
public:
  virtual ~Primitive();
  virtual  bool intersect(vec3 eye, vec3 ray_dir, double *t, vec3 *n, vec3 *tg, vec2* uv) {return true;}
  bool has_uv = false;
};

class Sphere : public Primitive {
public:
  virtual ~Sphere();
  bool intersect(vec3 eye, vec3 ray_dir, double *t, vec3 *n, vec3 *tg, vec2* uv);
private:
  glm::vec3 m_pos;
  double m_radius;
};

class Cube : public Primitive {
public:
  virtual ~Cube();
  bool intersect(vec3 eye, vec3 ray_dir, double *t, vec3 *n, vec3 *tg, vec2* uv);
};

class NonhierSphere : public Primitive {
public:
  NonhierSphere(const glm::vec3& pos, double radius)
    : m_pos(pos), m_radius(radius)
  {
  }
  virtual ~NonhierSphere();
  bool intersect(vec3 eye, vec3 ray_dir, double *t, vec3 *n, vec3 *tg, vec2* uv);

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
  bool intersect(vec3 eye, vec3 ray_dir, double *t, vec3 *n, vec3 *tg, vec2* uv);
  virtual ~NonhierBox();

private:
  bool isWithin(double posx, double posy, double xmin, double ymin, double xmax, double ymax);
  glm::vec3 m_pos;
  double m_size;
};
