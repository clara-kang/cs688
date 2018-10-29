// Fall 2018

#include "Primitive.hpp"
#include "polyroots.hpp"
#include <glm/glm.hpp>
#include <math.h>
#include <iostream>
#include <glm/gtx/string_cast.hpp>

using namespace std;

Primitive::~Primitive()
{

}

Sphere::~Sphere()
{
}

Cube::~Cube()
{
}

NonhierSphere::~NonhierSphere()
{

}
bool NonhierSphere::intersect(vec3 eye, vec3 ray_dir, double *t, vec3 *n) {
  //cout << glm::to_string(ray_dir) << endl;
  double A = pow(glm::length(ray_dir),2.0);
  double B = 2.0*glm::dot(ray_dir, (eye-m_pos));
  double C = pow(glm::length(eye-m_pos),2.0) - pow(m_radius,2.0);
  double roots[2];
  size_t num_roots = quadraticRoots(A,B,C,roots);
  if (num_roots == 1) {
    *t = roots[0];
  } else if (num_roots == 2) {
    *t = std::fmin(roots[0],roots[1]);
  } else {
    return false;
  }
  *n = glm::normalize(eye + (float)(*t)*ray_dir - m_pos);
  return true;
}

NonhierBox::~NonhierBox()
{
}
bool NonhierBox::intersect(vec3 eye, vec3 ray_dir, double *t, vec3 *n) {
  double xmin,xmax,ymin,ymax,zmin,zmax;
  double t_test, x, y;
  vec3 p_istn;
  xmin = m_pos[0]-m_size/2.0;
  xmax = m_pos[0]+m_size/2.0;
  ymin = m_pos[1]-m_size/2.0;
  ymax = m_pos[1]+m_size/2.0;
  zmin = m_pos[2]-m_size/2.0;
  zmax = m_pos[2]+m_size/2.0;
  *t = HUGE_VAL;
  // z min fixed
  t_test = (zmin - eye[2]) / ray_dir[2];
  p_istn = eye+ray_dir*(float)t_test;
  if (isWithin(p_istn[0], p_istn[1], xmin, ymin, xmax, ymax)) {
    *t = std::fmin(*t, t_test);
    *n = vec3(0.0, 0.0, -1.0);
  }
  // z max fixed
  t_test = (zmax - eye[2]) / ray_dir[2];
  p_istn = eye+ray_dir*(float)t_test;
  if (isWithin(p_istn[0], p_istn[1], xmin, ymin, xmax, ymax)) {
    *t = std::fmin(*t, t_test);
    *n = vec3(0.0, 0.0, 1.0);
  }
  // y min Fixed
  t_test = (ymin - eye[1]) / ray_dir[1];
  p_istn = eye+ray_dir*(float)t_test;
  if (isWithin(p_istn[0], p_istn[2], xmin, zmin, xmax, zmax)) {
    *t = std::fmin(*t, t_test);
    *n = vec3(0.0, -1.0, 0.0);
  }
  // y max Fixed
  t_test = (ymax - eye[1]) / ray_dir[1];
  p_istn = eye+ray_dir*(float)t_test;
  if (isWithin(p_istn[0], p_istn[2], xmin, zmin, xmax, zmax)) {
    *t = std::fmin(*t, t_test);
    *n = vec3(0.0, 1.0, 0.0);
  }
  // x min Fixed
  t_test = (xmin - eye[0]) / ray_dir[0];
  p_istn = eye+ray_dir*(float)t_test;
  if (isWithin(p_istn[1], p_istn[2], ymin, zmin, ymax, zmax)) {
    *t = std::fmin(*t, t_test);
    *n = vec3(-1.0, 0.0, 0.0);
  }
  // x max Fixed
  t_test = (xmax - eye[0]) / ray_dir[0];
  p_istn = eye+ray_dir*(float)t_test;
  if (isWithin(p_istn[1], p_istn[2], ymin, zmin, ymax, zmax)) {
    *t = std::fmin(*t, t_test);
    *n = vec3(1.0, 0.0, 0.0);
  }
  if (*t < HUGE_VAL) {
    return true;
  }
  return false;

}
bool NonhierBox::isWithin (double posx, double posy, double xmin, double ymin, double xmax, double ymax){
  return xmin <= posx && posx <= xmax && ymin <= posy && posy <= ymax;
}
