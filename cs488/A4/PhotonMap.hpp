// Fall 2018

#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <map>
#include <algorithm>

#include "SceneNode.hpp"
#include "GeometryNode.hpp"
#include "PhongMaterial.hpp"
#include "Light.hpp"
#include "Image.hpp"

struct Photon {
  vec3 pos;
  vec3 dir;
  vec3 color;
};

struct Cell {
  vec3 pos;
  float phi, theta;
};

struct TreeNode {
  char axis;
  float value;
  Photon *photon;
  TreeNode *left;
  TreeNode *right;
};



class PhotonMap {
public:
  virtual ~PhotonMap();

	PhotonMap(const std::list<Light *> & lights, const vec3 & eye,
    const vec3 & view_dir, double fov, SceneNode * root);
  void createProjMap();
  void castPhotons();
  void renderPhotonMap();
  void renderProjectionMap();
  void buildKdTree();
  void locatePhotons(const vec3 & position, float dist_square, int n,
  	std::vector<Photon *> *photons_found);
  void test();

  void renderKDtree();

private:
  std::map<int,std::vector<Cell>> projection_map;
  std::vector<Photon> photon_map;
  SceneNode * root;
  const std::list<Light *> & lights;
  const vec3 & eye;
  const vec3 & view_dir;
  double fov;
  TreeNode *kd_tree = NULL;

  void castPrimaryRay( SceneNode *root, const glm::vec3 & start, const glm::vec3 & ray_dir,
    Intersection *isect, mat4 T, mat4 T_n, GeometryNode ** obj, bool shadow_ray);
  void SurfaceInteraction(Dielectric *dielectric, const vec3 & normal, const vec3 & ray_dir,
    vec3 *new_ray_dir);
  TreeNode *buildKdTree(std::vector<Photon> photons);
  void freeKdTree(TreeNode *tree);
  void locatePhotons(const vec3 & position, float dist_square, TreeNode *tree, int n,
    std::vector<Photon *> *photons_found);

  void renderPhoton(Image *image, Photon *photon);
  void renderKDtreeRec(Image *image, TreeNode *tree);
};
