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



PhotonMap::PhotonMap(const std::list<Light *> & lights, const vec3 & eye,
	const vec3 & view_dir, double fov, SceneNode * root):
lights(lights),
eye(eye),
view_dir(view_dir),
fov(fov),
root(root),
kd_tree(NULL){
	srand(time(NULL));
};

PhotonMap::~PhotonMap(){
	freeKdTree(kd_tree);
}

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
		float inc_phi = 2.0f * INC_PHI / dim;
		float inc_theta = 2.0f * INC_THETA / dim;
		float phi, theta;
		float random_offset_phi, random_offset_theta;
		for (Cell cell : projection_map[light_index]) {
			for (int i = 0; i < dim; i++) {
				random_offset_phi = INC_PHI * getRandomNum()/5.0;
				for (int j = 0; j < dim; j++) {
					random_offset_theta = INC_THETA * getRandomNum()/5.0;
					phi = cell.phi - INC_PHI + random_offset_phi + i * inc_phi;
					theta = cell.theta - INC_THETA + random_offset_theta + j * inc_theta;
	        vec3 ray_dir = glm::normalize(vec3(sin(phi)*sin(theta), cos(phi), sin(phi)*cos(theta)));
					isect.t = HUGE_VAL;
					castPrimaryRay( root, light->position, ray_dir, &isect, mat4(1.0), mat4(1.0), &node);
					if (isect.t < HUGE_VAL) {
						Dielectric *dielectric = dynamic_cast<Dielectric *>(node->m_material);
						if (dielectric != NULL) {
							int cnt = 0;
							intersection = light->position + isect.t * ray_dir;
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
								photon.color = light->colour;
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
	cos_thetaT = sqrt(1-pow(sin_thetaT, 2.0));

	if (sin_thetaT >= 1.0) {
		r_avg = 1.0;
	} else {
		// compute r_avg
		double r_par = (eta_T * cos_thetaI - eta_I * cos_thetaT)/
			(eta_T * cos_thetaI + eta_I * cos_thetaT);
		double r_per = (eta_I * cos_thetaI - eta_T*cos_thetaT)/
			(eta_I * cos_thetaI + eta_T*cos_thetaT);
		r_avg = (pow(r_par,2.0) + pow(r_per,2.0))/2.0;
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

bool lessX(const Photon& photon1, const Photon& photon2) {
	return photon1.pos[0] < photon2.pos[0];
}

bool lessY(const Photon& photon1, const Photon& photon2) {
	return photon1.pos[2] < photon2.pos[2];
}

bool lessZ(const Photon& photon1, const Photon& photon2) {
	return photon1.pos[2] < photon2.pos[2];
}
static int leaves = 0;

void PhotonMap::buildKdTree() {
	kd_tree = buildKdTree(photon_map);
	cout << "photons: " << photon_map.size() << endl;
}

void PhotonMap::test() {
	Photon photon1 = {
		vec3 (0, -20, -20),
		vec3 (0,0,0),
	};
	Photon photon2 = {
		vec3 (0, 15, 15),
		vec3 (0,0,0),
	};
	Photon photon3 = {
		vec3 (0, -11, -11),
		vec3 (0,0,0),
	};
	Photon photon4 = {
		vec3 (0, 10, 10),
		vec3 (0,0,0),
	};
	Photon photon5 = {
		vec3 (0, -5, -5),
		vec3 (0,0,0),
	};
	vector<Photon> photonList;
	photon_map.push_back(photon1);
	photon_map.push_back(photon2);
	photon_map.push_back(photon3);
	photon_map.push_back(photon4);
	photon_map.push_back(photon5);
	buildKdTree();
	vector<Photon *> photons_nearby;
	locatePhotons(vec3 (0,0,0), 202, kd_tree, 2, &photons_nearby);
	cout << "photons_nearby" << endl;
	std::vector<Photon *>::iterator it = photons_nearby.begin();
	while(it != photons_nearby.end()) {
		cout << glm::to_string((*it)->pos) << endl;
		it ++;
	}
}

void PhotonMap::locatePhotons(const vec3 & position, float dist_square, int n,
	std::vector<Photon *> *photons_found){
		locatePhotons(position, dist_square, kd_tree, n, photons_found);
}

void PhotonMap::locatePhotons(const vec3 & position, float dist_square, TreeNode *tree,
	int n, std::vector<Photon *> *photons_found) {
		if (tree->left == NULL && tree->right == NULL) {
			float dist_to_photon = glm::length(position - (tree->photon)->pos);
			if ( pow(dist_to_photon, 2.0) < dist_square ) {
				(*photons_found).push_back(tree->photon);
				std::push_heap((*photons_found).begin(), (*photons_found).end(),
					[&position](const Photon *photon1, const Photon *photon2) -> bool {
						return glm::length(photon1->pos - position) < glm::length(photon2->pos - position);
					});
				if ((*photons_found).size() > n) {
					std::pop_heap((*photons_found).begin(), (*photons_found).end());
					(*photons_found).pop_back();
				}
			}
		} else {
			float dist_to_plane;
			switch (tree->axis) {
				case 'x':
					dist_to_plane = position[0] - tree->value;
					break;
				case 'y':
					dist_to_plane = position[1] - tree->value;
					break;
				case 'z':
					dist_to_plane = position[2] - tree->value;
			}
			//on the left of plane
			if (dist_to_plane < 0) {
				locatePhotons(position, dist_square, tree->left, n, photons_found);
				if (pow(dist_to_plane,2.0) < dist_square) {
					locatePhotons(position, dist_square, tree->right, n, photons_found);
				}
			} else {
				locatePhotons(position, dist_square, tree->right, n, photons_found);
				if (pow(dist_to_plane,2.0) < dist_square) {
					locatePhotons(position, dist_square, tree->left, n, photons_found);
				}
			}
		}
}

void PhotonMap::freeKdTree(TreeNode *tree) {
	if (tree != NULL) {
		TreeNode *left = tree->left;
		TreeNode *right = tree->right;
		freeKdTree ( tree->left );
		freeKdTree ( tree->right );
		delete tree->photon;
		delete tree;
	}
}

TreeNode *PhotonMap::buildKdTree(std::vector<Photon> photons) {
	TreeNode *node = new TreeNode();
	if (photons.size() == 1) {
		leaves ++;
		node->photon = new Photon();
		*(node->photon) = photons.at(0);
		node->left = NULL;
		node->right = NULL;
		return node;
	}
	// get axis with largest range
	vector<Photon>::iterator minX = std::min_element(photons.begin(), photons.end(),lessX);
	vector<Photon>::iterator maxX = std::max_element(photons.begin(), photons.end(),lessX);
	vector<Photon>::iterator minY = std::min_element(photons.begin(), photons.end(),lessY);
	vector<Photon>::iterator maxY = std::max_element(photons.begin(), photons.end(),lessY);
	vector<Photon>::iterator minZ = std::min_element(photons.begin(), photons.end(),lessZ);
	vector<Photon>::iterator maxZ = std::max_element(photons.begin(), photons.end(),lessZ);
	float x_range_size = (*maxX).pos[0] - (*minX).pos[0];
	float y_range_size = (*maxY).pos[1] - (*minY).pos[1];
	float z_range_size = (*maxZ).pos[2] - (*minZ).pos[2];

	int mid = (int)((float)photons.size()/2.0f);
	float mid_val;

	if (x_range_size > fmax(y_range_size,z_range_size)){
		std::sort(photons.begin(), photons.end(), lessX);
		mid_val = (photons.at(mid-1)).pos[0];
		node->axis = 'x';
	} else if (y_range_size > fmax(x_range_size,z_range_size)) {
		std::sort(photons.begin(), photons.end(), lessY);
		mid_val = (photons.at(mid-1)).pos[1];
		node->axis = 'y';
	} else {
		std::sort(photons.begin(), photons.end(), lessZ);
		mid_val = (photons.at(mid-1)).pos[2];
		node->axis = 'z';
	}
	node->value = mid_val;
	// balance child
	vector<Photon> left(mid);
	vector<Photon> right(photons.size()-mid);
	std::copy(photons.begin(), photons.begin()+mid, left.begin());
	std::copy(photons.begin()+mid, photons.end(), right.begin());
	node->left = buildKdTree(left);
	node->right = buildKdTree(right);
	return node;
}

void PhotonMap::renderPhotonMap() {
  Image image(256,256);
  float size = 256.0f;
  float half_fov = (fov/2.0f) * PI/ 180.0f;
  float half_cos_fov = cos(half_fov);
  float half_sin_fov = sin(half_fov);
	vec3 to_photon;
  int i,j;
  int count = 0;
	for (Photon photon : photon_map) {
		to_photon = photon.pos - eye;
		float y_cos = glm::dot(glm::normalize(vec3(0,to_photon.y, to_photon.z)), view_dir);
		float x_cos = glm::dot(glm::normalize(vec3(to_photon.x, 0,to_photon.z)), view_dir);
		if (y_cos > half_cos_fov && x_cos > half_cos_fov) {
			float y_sin = sqrt(1-pow(y_cos, 2.0));
			float x_sin = sqrt(1-pow(x_cos, 2.0));
			if (to_photon.y < 0) {
				y_sin = -y_sin;
			}
			if (to_photon.x < 0) {
				x_sin = -x_sin;
			}
			i = (int)((x_sin / half_sin_fov) * size / 2.0 + size / 2.0);
			j = (int)(-(y_sin / half_sin_fov) * size / 2.0 + size / 2.0);
			for ( int k = 0; k < 3; k++) {
				image((uint)i, (uint)j, k) = 1.0;
			}
			count ++;
		}
	}
  image.savePng( "photonmap.png" );
}

void PhotonMap::renderProjectionMap() {
  Image image(256,256);
  float size = 256.0f;
  float half_fov = (fov/2.0f) * PI/ 180.0f;
  float half_cos_fov = cos(half_fov);
  float half_sin_fov = sin(half_fov);
	float half_tan_fov = tan(half_fov);
	vec3 to_cell;
  int i,j;
  int count = 0;
	int light_index = 0;
	for (Light *light : lights) {
		for (Cell cell : projection_map[light_index]) {
			to_cell = cell.pos - eye;
			float y_cos = glm::dot(glm::normalize(vec3(0,to_cell.y, to_cell.z)), view_dir);
			float x_cos = glm::dot(glm::normalize(vec3(to_cell.x, 0,to_cell.z)), view_dir);
			float y_ang = acos(y_cos);
			float x_ang = acos(x_cos);
			// cout << "y_cos: " << y_cos << ",x_cos: " << x_cos << endl;
			if (y_cos > half_cos_fov && x_cos > half_cos_fov) {
				float y_tan = tan(y_ang);
				float x_tan = tan(x_ang);
				if (to_cell.y < 0) {
					y_tan = -y_tan;
				}
				if (to_cell.x < 0) {
					x_tan = -x_tan;
				}
				i = (int)((x_tan / half_tan_fov) * size / 2.0 + size / 2.0);
				j = (int)(-(y_tan / half_tan_fov) * size / 2.0 + size / 2.0);
				for ( int k = 0; k < 3; k++) {
					image((uint)i, (uint)j, k) = 1.0;
				}
				count ++;
			}
		}
	}
  image.savePng( "projection_map.png" );
}
