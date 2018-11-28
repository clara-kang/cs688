// Fall 2018

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <glm/ext.hpp>
#include <glm/gtx/string_cast.hpp>
#include <time.h>

#include "PhotonMap.hpp"

using namespace std;
using namespace glm;

#define PI 3.14159
#define EPSILON 1.0e-1
#define DIELECTRIC 2
#define GLOSSY 3
#define DIFFUSE 1

static const int NUM_INC_PROJ_MAP = 100;
static const int REFLECT_MAX_TIMES = 3;
static const float INC_PHI = PI / NUM_INC_PROJ_MAP;
static const float INC_THETA = 2.0f * PI / NUM_INC_PROJ_MAP;



PhotonMap::PhotonMap(const std::list<Light *> & lights, SceneNode * root):
lights(lights),
root(root){
	srand(time(NULL));
};

float getRandomNum() {
	return (rand()%10+1.0f)/10.0f;
}

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

void PhotonMap::createProjMap() {
  Intersection isect = Intersection();
  GeometryNode *node;
	int light_index = 0;
  for (Light *light : lights) {
    if (light->photon_num == 0) {
      break;
    }
    for(int i = 0; i < NUM_INC_PROJ_MAP; i++) {
      for(int j = 0; j < NUM_INC_PROJ_MAP; j++) {
        float phi = i * INC_PHI;
        float theta = j * INC_THETA;
        vec3 ray_dir = glm::normalize(vec3(sin(phi)*sin(theta), cos(phi), sin(phi)*cos(theta)));
        isect.t = HUGE_VAL;
        castPrimaryRay( root, light->position, ray_dir, &isect, mat4(1.0), mat4(1.0), &node);
        if (isect.t < HUGE_VAL) {
					Dielectric *dielectric = dynamic_cast<Dielectric *>(node->m_material);
					if (dielectric != NULL) {
	          Cell cell;
	          cell.phi = phi;
	          cell.theta = theta;
	          cell.pos = light->position + isect.t * ray_dir;
	          (projection_map[light_index]).push_back(cell);
					}
        }
      }
    }
		light_index ++;
  }
}

// diffuse: 1, dielectric: 2, glossy: 3
int matType(Material *mat) {
	Dielectric *dielectric = dynamic_cast<Dielectric *>(mat);
	Glossy *glossy = dynamic_cast<Glossy *>(mat);
	if (dielectric != NULL) {
		return DIELECTRIC;
	} else if (glossy != NULL) {
		return GLOSSY;
	} else {
		return DIFFUSE;
	}
}
void PhotonMap::castPhotons() {
	int light_index = 0;
	Intersection isect = Intersection();
	GeometryNode *node;
	vec3 rf_ray_dir, intersection;
	for (Light *light : lights) {
		int photons_per_cell = light->photon_num / (projection_map[light_index]).size();
		float dim = sqrt(photons_per_cell);
		float inc_phi = INC_PHI / dim;
		float inc_theta = INC_THETA / dim;
		float phi, theta;
		for (Cell cell : projection_map[light_index]) {
			for (int i = 0; i < dim; i++) {
				for (int j = 0; j < dim; j++) {
					phi = cell.phi - INC_PHI/2.0f + i * inc_phi;
					theta = cell.theta - INC_THETA/2.0f + j * inc_theta;
	        vec3 ray_dir = glm::normalize(vec3(sin(phi)*sin(theta), cos(phi), sin(phi)*cos(theta)));
					isect.t = HUGE_VAL;
					castPrimaryRay( root, light->position, ray_dir, &isect, mat4(1.0), mat4(1.0), &node);
					if (isect.t < HUGE_VAL) {
						Dielectric *dielectric = dynamic_cast<Dielectric *>(node->m_material);
						if (dielectric != NULL) {
							int cnt = 0;
							intersection = light->position + isect.t * ray_dir;
							bool store = false;
							do {
								SurfaceInteraction(dielectric, isect.normal, ray_dir, &ray_dir);
								isect.t = HUGE_VAL;
								castPrimaryRay( root, intersection, ray_dir, &isect, mat4(1.0), mat4(1.0), &node);
								if (isect.t == HUGE_VAL) {
									break;
								} else {
									intersection = intersection + isect.t * ray_dir;
									if (matType(node->m_material) != DIELECTRIC) {
										break;
									}
								}
								cnt ++;
							} while(cnt <= REFLECT_MAX_TIMES);
							// store photon
							if (isect.t < HUGE_VAL && matType(node->m_material) == DIFFUSE) {
								Photon photon;
								photon.dir = ray_dir;
								photon.pos = intersection;
								photon_map.push_back(photon);
							}
						}
					}
				}
			}
		}
	}
}

void PhotonMap::SurfaceInteraction(
	Dielectric *dielectric,
	const vec3 & surface_normal,
	const vec3 & ray_dir,
	vec3 *new_ray_dir
) {
	vec3 n_normal = glm::normalize(surface_normal);
	vec3 normal;
	double cos_thetaI, sin_thetaI, sin_thetaT, cos_thetaT;
	double r_avg;
	cos_thetaI = glm::dot(n_normal, -ray_dir);
	bool from_inside = cos_thetaI < 0;
	double eta_T, eta_I;
	if (from_inside) {
		eta_T = 1.0;
		eta_I = dielectric->m_rf_index;
		cos_thetaI = - cos_thetaI;
		normal = -n_normal;
	} else {
		eta_I = 1.0;
		eta_T = dielectric->m_rf_index;
		normal = n_normal;
	}
	sin_thetaI = sqrt(1-pow(cos_thetaI, 2.0));
	sin_thetaT = sin_thetaI * eta_I/eta_T;

	if (sin_thetaT >= 1.0) {
		r_avg = 1.0;
	} else {
		// compute r_avg
		double r_par = (eta_T * cos_thetaI - eta_I * cos_thetaT)/
			(eta_T * cos_thetaI + eta_I * cos_thetaT);
		double r_per = (eta_I * cos_thetaI - eta_T*cos_thetaT)/
			(eta_I * cos_thetaI + eta_T*cos_thetaT);
		r_avg = (pow(r_par,2.0) + pow(r_per,2.0))/2.0;
		r_avg = std::fmin(std::fmax(r_avg, 0.0), 1.0);
	}
	float random_num = getRandomNum();
	if (0 <= random_num && random_num < 1-r_avg) {
		// transmission
		*new_ray_dir = glm::normalize(ray_dir + normal*cos_thetaI) * sin_thetaT - glm::normalize(normal*cos_thetaI) * cos_thetaT;
	} else {
		// reflection
		*new_ray_dir = glm::normalize(2.0*cos_thetaI*normal + ray_dir);
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
  cout << "map size: " << projection_map.size() << endl;
  int count = 0;
	for (Photon photon : photon_map) {
		float y_cos = glm::dot(glm::normalize(vec3(0,photon.pos.y, photon.pos.z)), vec3(0,0,-1));
		float x_cos = glm::dot(glm::normalize(vec3(photon.pos.x, 0,photon.pos.z)), vec3(0,0,-1));
		// cout << "y_cos: " << y_cos << ",x_cos: " << x_cos << endl;
		if (y_cos > half_cos_fov && x_cos > half_cos_fov) {
			float y_sin = sqrt(1-pow(y_cos, 2.0));
			float x_sin = sqrt(1-pow(x_cos, 2.0));
			if (photon.pos.y < 0) {
				y_sin = -y_sin;
			}
			if (photon.pos.x < 0) {
				x_sin = -x_sin;
			}
			// cout << "y_sin: " << y_sin << ",x_sin: " << x_sin << endl;
			i = (int)((x_sin / half_sin_fov) * size / 2.0 + size / 2.0);
			j = (int)(-(y_sin / half_sin_fov) * size / 2.0 + size / 2.0);
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

void PhotonMap::renderProjectionMap() {
  Image image(256,256);
  vec3 eye = vec3(0,0,0);
  float fov = 180.0f;
  float size = 256.0f;
  float half_fov = (fov/2.0f) * PI/ 180.0f;
  float half_cos_fov = cos(half_fov);
  float half_sin_fov = sin(half_fov);
  int i,j;
  int count = 0;
	int light_index = 0;
	for (Light *light : lights) {
		for (Cell cell : projection_map[light_index]) {
			float y_cos = glm::dot(glm::normalize(vec3(0,cell.pos.y, cell.pos.z)), vec3(0,0,-1));
			float x_cos = glm::dot(glm::normalize(vec3(cell.pos.x, 0,cell.pos.z)), vec3(0,0,-1));
			// cout << "y_cos: " << y_cos << ",x_cos: " << x_cos << endl;
			if (y_cos > half_cos_fov && x_cos > half_cos_fov) {
				float y_sin = sqrt(1-pow(y_cos, 2.0));
				float x_sin = sqrt(1-pow(x_cos, 2.0));
				if (cell.pos.y < 0) {
					y_sin = -y_sin;
				}
				if (cell.pos.x < 0) {
					x_sin = -x_sin;
				}
				// cout << "y_sin: " << y_sin << ",x_sin: " << x_sin << endl;
				i = (int)((x_sin / half_sin_fov) * size / 2.0 + size / 2.0);
				j = (int)(-(y_sin / half_sin_fov) * size / 2.0 + size / 2.0);
				// cout << "i: " << i << ",j: " << j << endl;
				for ( int k = 0; k < 3; k++) {
					image((uint)i, (uint)j, k) = 1.0;
				}
				count ++;
			}
		}
	}
  cout << "count proj: " << count << endl;
  image.savePng( "projection_map.png" );
}
