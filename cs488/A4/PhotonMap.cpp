// Fall 2018

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <glm/ext.hpp>
#include <glm/gtx/string_cast.hpp>

using namespace std;
using namespace glm;

#define PI 3.14159
#define EPSILON 1.0e-1

// #include "cs488-framework/ObjFileDecoder.hpp"
#include "PhotonMap.hpp"

PhotonMap::PhotonMap(const std::list<Light *> & lights, SceneNode * root):
lights(lights),
root(root){

};

void PhotonMap::castPrimaryRay(
	SceneNode *root,
	const glm::vec3 & start,
	const glm::vec3 & ray_dir,
	Intersection *isect,
	mat4 T,
	mat4 T_n,
	GeometryNode ** obj
) {
	mat4 T_new = root->invtrans * T;
	mat4 T_n_new = T_n * glm::transpose(root->invtrans);
	if ( root -> m_nodeType == NodeType::GeometryNode ){
		GeometryNode * gnode = static_cast<GeometryNode *>(root);
		Primitive *prim = gnode->m_primitive;
		Intersection tmp_isect = Intersection();
		// test dielectric
			bool intersect = prim->intersect(vec3(T_new*vec4(start,1.0)),
				vec3(T_new*vec4(ray_dir,0.0)), &tmp_isect);
			if (intersect && tmp_isect.t < isect->t) {
				isect->t = tmp_isect.t;
				isect->normal = vec3(T_n_new*vec4(tmp_isect.normal, 0.0));
				isect->tangent = vec3(glm::inverse(T_new)*vec4(tmp_isect.tangent, 0.0));
				isect->uv = tmp_isect.uv;
				*obj = gnode;
			}
	}
	for (SceneNode *child : root->children) {
		castPrimaryRay(child, start, ray_dir, isect, T_new, T_n_new, obj);
	}
}

void PhotonMap::castPhotons() {
  Intersection isect = Intersection();
  GeometryNode *node;
  for (Light *light : lights) {
    cout << "num: " << light->photon_num << endl;
    if (light->photon_num == 0) {
      break;
    }
    int num_inc = sqrt(light->photon_num);
    float inc_phi = PI / num_inc;
    float inc_theta = 2.0f * PI / num_inc;
    for(int i = 0; i < num_inc; i++) {
      for(int j = 0; j < num_inc; j++) {
        float phi = i * inc_phi;
        float theta = j * inc_theta;
        vec3 ray_dir = glm::normalize(vec3(sin(phi)*sin(theta), cos(phi), sin(phi)*cos(theta)));
        isect.t = HUGE_VAL;
        castPrimaryRay( root, light->position, ray_dir, &isect, mat4(1.0), mat4(1.0), &node);
        if (isect.t < HUGE_VAL) {
          Photon photon;
          photon.phi = phi;
          photon.theta = theta;
          photon.pos = light->position + isect.t * ray_dir;
          m_photon_list.push_back(photon);
        }
      }
    }
  }
}

void PhotonMap::renderPhotonMap() {
  Image image(256,256);
  vec3 eye = vec3(0,0,0);
  float fov = 180.0f;
  float size = 256.0f;
  float half_fov = (fov/2.0f) * PI/ 180.0f;
  float half_cos_fov = cos(half_fov);
  float half_sin_fov = sin(half_fov);
  int i,j;
  cout << "map size: " << m_photon_list.size() << endl;
  int count = 0;
  for (Photon photon : m_photon_list) {
    float y_cos = glm::dot(glm::normalize(vec3(0,photon.pos.y, photon.pos.z)), vec3(0,0,-1));
    float x_cos = glm::dot(glm::normalize(vec3(photon.pos.x, 0,photon.pos.z)), vec3(0,0,-1));
    // cout << "y_cos: " << y_cos << ",x_cos: " << x_cos << endl;
    if (y_cos > half_cos_fov && x_cos > half_cos_fov) {
      float y_sin = sqrt(1-pow(y_cos, 2.0));
      float x_sin = sqrt(1-pow(x_cos, 2.0));
      if (photon.pos.y > 0) {
        y_sin = -y_sin;
      }
      if (photon.pos.x < 0) {
        x_sin = -x_sin;
      }
      // cout << "y_sin: " << y_sin << ",x_sin: " << x_sin << endl;
      i = (int)((x_sin / half_sin_fov) * size / 2.0 + size / 2.0);
      j = (int)((y_sin / half_sin_fov) * size / 2.0 + size / 2.0);
      // cout << "i: " << i << ",j: " << j << endl;
      for ( int k = 0; k < 3; k++) {
        image((uint)i, (uint)j, k) = 1.0;
      }
      count ++;
    }
  }
  cout << "count: " << count << endl;
  image.savePng( "photonmap.png" );
}
