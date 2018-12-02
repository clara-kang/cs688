// Fall 2018

#include "Primitive.hpp"
#include "polyroots.hpp"
#include <glm/glm.hpp>
#include <math.h>
#include <iostream>
#include <glm/gtx/string_cast.hpp>

#define PI 3.14159

using namespace std;

Primitive::~Primitive()
{

}

Sphere::~Sphere()
{
}
bool Sphere::intersect(vec3 eye, vec3 ray_dir, Intersection *isect) {
  NonhierSphere ns (vec3(0.0,0.0,0.0), 1.0);
  return ns.intersect(eye, ray_dir, isect);
}

Cube::~Cube()
{
}
bool Cube::intersect(vec3 eye, vec3 ray_dir, Intersection *isect) {
  NonhierBox nb (vec3(0.0,0.0,0.0), 1.0);
  return nb.intersect(eye, ray_dir, isect);
}

NonhierSphere::~NonhierSphere()
{

}
bool NonhierSphere::intersect(vec3 eye, vec3 ray_dir, Intersection *isect) {
  //cout << glm::to_string(ray_dir) << endl;
  double A = pow(glm::length(ray_dir),2.0);
  double B = 2.0*glm::dot(ray_dir, (eye-m_pos));
  double C = pow(glm::length(eye-m_pos),2.0) - pow(m_radius,2.0);
  double roots[2];
  double s_root, b_root;
  size_t num_roots = quadraticRoots(A,B,C,roots);
  if (num_roots == 1) {
    isect->t = roots[0];
  } else if (num_roots == 2) {
    s_root = std::fmin(roots[0],roots[1]);
    b_root = std::fmax(roots[0],roots[1]);
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
  if (isect->t > 0) {
    vec3 normal = eye + (float)(isect->t)*ray_dir - m_pos;
    isect->normal = glm::normalize(normal);
    double u = 0.5 + atan2(normal[0],normal[2])/(2*PI);
    double v = 0.5 + asin(normal[1]/m_radius)/PI;
    isect->uv = vec2(u,v);
    isect->tangent = vec3(-(isect->normal)[2],0,(isect->normal)[0]);
    return true;
  }
  return false;
}

NonhierBox::~NonhierBox()
{
}
bool NonhierBox::intersect(vec3 eye, vec3 ray_dir, Intersection *isect) {
  double xmin,xmax,ymin,ymax,zmin,zmax;
  double t_test, x, y;
  int intersectFace = -1; //1:Zmin 2:Zmax 3:Ymin 4:Ymax 5:Xmin 6:Xmax
  vec3 p_istn;
  xmin = m_pos[0];
  xmax = m_pos[0]+m_size;
  ymin = m_pos[1];
  ymax = m_pos[1]+m_size;
  zmin = m_pos[2];
  zmax = m_pos[2]+m_size;
  // z min fixed
  t_test = (zmin - eye[2]) / ray_dir[2];
  p_istn = eye+ray_dir*(float)t_test;
  if (isWithin(p_istn[0], p_istn[1], xmin, ymin, xmax, ymax)) {
    if (t_test < isect->t && t_test > 0) {
      isect->t = t_test;
      intersectFace = 1;
    }
  }
  // z max fixed
  t_test = (zmax - eye[2]) / ray_dir[2];
  p_istn = eye+ray_dir*(float)t_test;
  if (isWithin(p_istn[0], p_istn[1], xmin, ymin, xmax, ymax)) {
    if (t_test < isect->t && t_test > 0) {
      isect->t = t_test;
      intersectFace = 2;
    }
  }
  // y min Fixed
  t_test = (ymin - eye[1]) / ray_dir[1];
  p_istn = eye+ray_dir*(float)t_test;
  if (isWithin(p_istn[0], p_istn[2], xmin, zmin, xmax, zmax)) {
    if (t_test < isect->t && t_test > 0) {
      isect->t = t_test;
      intersectFace = 3;
    }
  }
  // y max Fixed
  t_test = (ymax - eye[1]) / ray_dir[1];
  p_istn = eye+ray_dir*(float)t_test;
  if (isWithin(p_istn[0], p_istn[2], xmin, zmin, xmax, zmax)) {
    if (t_test < isect->t && t_test > 0) {
      isect->t = t_test;
      intersectFace = 4;
    }
  }
  // x min Fixed
  t_test = (xmin - eye[0]) / ray_dir[0];
  p_istn = eye+ray_dir*(float)t_test;
  if (isWithin(p_istn[1], p_istn[2], ymin, zmin, ymax, zmax)) {
    if (t_test < isect->t && t_test > 0) {
      isect->t = t_test;
      intersectFace = 5;
    }
  }
  // x max Fixed
  t_test = (xmax - eye[0]) / ray_dir[0];
  p_istn = eye+ray_dir*(float)t_test;
  if (isWithin(p_istn[1], p_istn[2], ymin, zmin, ymax, zmax)) {
    if (t_test < isect->t && t_test > 0) {
      isect->t = t_test;
      intersectFace = 6;
    }
  }
  switch (intersectFace) {
    case 1:
      isect->normal = vec3(0.0, 0.0, -1.0);
      break;
    case 2:
      isect->normal = vec3(0.0, 0.0, +1.0);
      break;
    case 3:
      isect->normal = vec3(0.0, -1.0, 0.0);
      break;
    case 4:
      isect->normal = vec3(0.0, 1.0, 0.0);
      break;
    case 5:
      isect->normal = vec3(-1.0, 0.0, 0.0);
      break;
    case 6:
      isect->normal = vec3(1.0, 0.0, 0.0);
      break;
  }
  if (intersectFace == 1 || intersectFace == 2) {
    double intersectX = eye[0] + (isect->t)*ray_dir[0];
    double intersectY = eye[1] + (isect->t)*ray_dir[1];
    isect->uv = vec2(intersectX, intersectY);
    isect->tangent = vec3(1.0, 0.0, 0.0);
  } else if (intersectFace == 3 || intersectFace == 4) {
    double intersectX = eye[0] + (isect->t)*ray_dir[0];
    double intersectZ = eye[2] + (isect->t)*ray_dir[2];
    isect->uv = vec2(intersectX, intersectZ);
    isect->tangent = vec3(0.0, 0.0, 1.0);
  } else if (intersectFace == 5 || intersectFace == 6) {
    double intersectY = eye[1] + (isect->t)*ray_dir[1];
    double intersectZ = eye[2] + (isect->t)*ray_dir[2];
    isect->uv = vec2(intersectY, intersectZ);
    isect->tangent = vec3(0.0, 1.0, 0.0);
  }
  if (isect->t < HUGE_VAL) {
    return true;
  }
  return false;

}
bool NonhierBox::isWithin (double posx, double posy, double xmin, double ymin, double xmax, double ymax){
  return xmin <= posx && posx <= xmax && ymin <= posy && posy <= ymax;
}
