// Fall 2018

#pragma once

#include <glm/glm.hpp>
#include <map>
#include <iostream>
#include <vector>

#include "SceneNode.hpp"
#include "GeometryNode.hpp"
#include "PhongMaterial.hpp"
#include "Light.hpp"
#include "Image.hpp"
using namespace std;
class A4 {
struct image_data {
	int x;
	int y;
	int channel;
	unsigned char *im_data;

	glm::vec3 get_color_at_loc(glm::vec2 uv) {
		// cout << "u: " << uv[0] << ", v: " << uv[1] << endl;
		int i = std::max(0, (int)(uv[0] * x));
		int j = std::max(0, (int)(uv[1] * y));
		i = std::min(i, x-1);
		j = std::min(j, y-1);
		unsigned char* pixelOffset = im_data + (int)(i + x * j) * channel;
		return glm::vec3((uint)pixelOffset[0]/255.0,(uint)pixelOffset[1]/255.0,(uint)pixelOffset[2]/255.0);
	}
};

public:
	A4(
		// What to render
		SceneNode * root,

		// Image to write to, set to a given width and height
		Image & image,

		// Viewing parameters
		const glm::vec3 & eye,
		const glm::vec3 & view,
		const glm::vec3 & up,
		double fovy,

		// Lighting parameters
		const glm::vec3 & ambient,
		const std::list<Light *> & lights
);

~A4();

void A4_Render();
private:
	SceneNode * root;

	// Image to write to, set to a given width and height
	Image & image;

	// Viewing parameters
	const glm::vec3 & eye;
	const glm::vec3 & view;
	const glm::vec3 & up;
	double fovy;

	// Lighting parameters
	const glm::vec3 & ambient;
	const std::list<Light *> & lights;
	double pixsize_over_d;
	glm::vec3 right;
	bool *not_bg_map;
	double w,h;

	glm::vec3 getColor (
		Intersection &isect,
		const vec3 & eye,
		const vec3 & ray_dir,
		Material *material,
		int count);

	void A4_Render_pixel_rec(
			SceneNode *root,
			const glm::vec3 & start,
			const glm::vec3 & ray_dir,
			Intersection *isect,
			mat4 T,
			mat4 T_n,
			GeometryNode **obj
	);
	void A4_adaptive_sampling(
		Image & samples
	);
	glm::vec3 A4_sample_one_dir(
		double i, double j, bool resample
	);

	void reflectAndTransmit(
		const vec3 & n_normal,
		const vec3 & ray_dir,
		const vec3 & intersection,
		const vec3 & diffuse_color,
		vec3 *reflection_contrib,
		vec3 *transmission_contrib,
		PhongMaterial & phongMat,
		int count
	);

	bool A4_pixel_is_edge (glm::vec3 avg, glm::vec3 color);
	glm::vec3 get_ray_dir(double i, double j);
	glm::vec3 get_neighbor_pixel_avg(int i, int j, Image & samples);
	glm::vec3 recursive_sampling( Image & samples,double i,double j, int level, vec3 avg);
	glm::vec3 readNormalMap(string mapName, vec2 uv);
	glm::vec3 readTextureMap(string mapName, vec2 uv);

	std::map<std::string, image_data> textures;
	std::map<std::string, image_data> normal_maps;
	std::vector<double> randoms;
	std::vector<double> randomr;
};
