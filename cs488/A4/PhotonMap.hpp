// Fall 2018

#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "SceneNode.hpp"
#include "GeometryNode.hpp"
#include "PhongMaterial.hpp"
#include "Light.hpp"
#include "Image.hpp"

struct Photon {
  vec3 pos;
  float p[4];
  float phi, theta;
  short flag;
};

class PhotonMap {
public:
  virtual ~PhotonMap(){};

	PhotonMap(const std::list<Light *> & lights, SceneNode * root);
  void castPhotons();
  void castPrimaryRay( SceneNode *root, const glm::vec3 & start, const glm::vec3 & ray_dir,
  	Intersection *isect, mat4 T, mat4 T_n, GeometryNode ** obj);
  void renderPhotonMap();

private:
  std::vector<Photon> m_photon_list;
  SceneNode * root;
  const std::list<Light *> & lights;
};
