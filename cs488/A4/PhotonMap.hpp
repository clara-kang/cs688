// Fall 2018

#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <map>

#include "SceneNode.hpp"
#include "GeometryNode.hpp"
#include "PhongMaterial.hpp"
#include "Light.hpp"
#include "Image.hpp"

struct Photon {
  vec3 pos;
  vec3 dir;
  // float p[4];
  // float phi, theta;
  short flag;
};

struct Cell {
  vec3 pos;
  float phi, theta;
};

class PhotonMap {
public:
  virtual ~PhotonMap(){};

	PhotonMap(const std::list<Light *> & lights, const vec3 & eye,
    const vec3 & view_dir, double fov, SceneNode * root);
  void createProjMap();
  void castPhotons();
  void renderPhotonMap();
  void renderProjectionMap();

private:
  std::map<int,std::vector<Cell>> projection_map;
  std::vector<Photon> photon_map;
  SceneNode * root;
  const std::list<Light *> & lights;
  const vec3 & eye;
  const vec3 & view_dir;
  double fov;

  void castPrimaryRay( SceneNode *root, const glm::vec3 & start, const glm::vec3 & ray_dir,
    Intersection *isect, mat4 T, mat4 T_n, GeometryNode ** obj);
  void SurfaceInteraction(Dielectric *dielectric, const vec3 & normal, const vec3 & ray_dir,
    vec3 *new_ray_dir);
};
