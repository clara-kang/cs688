// Fall 2018

#include <glm/ext.hpp>

using namespace glm;
using namespace std;

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "A4.hpp"
#include <time.h>

#define PI 3.14159
#define EPSILON 1.0e-1

static const char *BG_FILE_NAME = "bg.jpg";
static const double COLOR_THRESHOLD = 0.01;
static const double REFLECT_MAX_TIMES = 3;
static const double RESAMPLE_LEVEL = 4;
static unsigned char *bg_data;

// TODO: Parse params instead
static const bool ADAPTIVE_SAMPLING = false;
static const bool SOFT_SHADOW = false;
static const bool FRESNEL = true;
static const bool PHOTON_MAP = true;
static const bool DEPTH_OF_FIELD = false;
static const double LIGHT_RADIUS = 0.05;
static const double GLOSSY_REFL_FRACTION = 0.4;
static const double SOFT_SHADOW_N = 10;
static const double GLOSSY_REFL_N = 10;
static const double DEPTH_OF_FIELD_N = 10;
static const double APERTURE_SIZE = 100.0;
static const double FOCUS = -0.0;
static int bg_x, bg_y, channels;

A4::A4(
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
):
root(root),
image(image),
eye(eye),
view(view),
up(up),
fovy(fovy),
ambient(ambient),
lights(lights),
photon_map(PhotonMap(lights, root)) {
	// load bg image
	bg_data = stbi_load(BG_FILE_NAME, &bg_x, &bg_y, &channels, 0);
	pixsize_over_d = 2.0 * tan(fovy*PI/(2.0*180.0)) / image.height();
	right = glm::normalize(glm::cross(view, up));
	not_bg_map = new bool[(int)(image.width()*image.height())];
	w = image.width();
	h = image.height();
	srand(time(NULL));
	for (int i = 0; i < SOFT_SHADOW_N; i++) {
		double r1 = (rand()%10+1.0)/10.0;
		double r2 = (rand()%10+1.0)/10.0;
		randoms.push_back(r1);
		randomr.push_back(r2);
	}
};

A4::~A4() {
	// for (std::map<std::string,image_data>::iterator it=textures.begin(); it!=textures.end(); ++it) {
	// 	stbi_image_free((it->second).im_data);
	// }
}

void A4::A4_Render(
) {
	// Get primary samples
	Image primary_samples( w, h);

	// generate rays
	vec3 normal, intersection;
	GeometryNode *obj;
	double t;

	// create photon map
	if (PHOTON_MAP) {
		photon_map.createProjMap();
		// photon_map.castPhotons();
		// photon_map.renderPhotonMap();
		photon_map.renderProjectionMap();
		return;
	}
	for (int i = 0; i < w; i++) {
		for (int j = 0; j < h; j++) {
			// cout << "i: " << i << ",j: " << j << endl;
			vec3 color = A4_sample_one_pixel((double)i,(double)j, false);
			for (int k = 0; k < 3; k++) {
					primary_samples((uint)i, (uint)j, k) = color[k];
			}
		}
	}
	// todo: improve on efficiency
	if (!ADAPTIVE_SAMPLING) {
		image = primary_samples;
	} else {
		A4_adaptive_sampling(primary_samples);
	}
	// free bg Image
	stbi_image_free(bg_data);
}

glm::vec3 A4::get_ray_dir(double i, double j) {
	vec3 ray_dir = ((-w/2.0 + i)*right + (h/2.0 - j)*up) *
		pixsize_over_d + view;
	ray_dir = glm::normalize(ray_dir);
	return ray_dir;
}

glm::vec3 A4::A4_sample_one_dir(vec3 & ray_dir, vec3 & start, int i, int j, bool resample) {
	Intersection isect = Intersection();
	GeometryNode *obj;
	A4_Render_pixel_rec (false, root, start, ray_dir, &isect, mat4(1.0), mat4(1.0), &obj);
	// if intersect object
	// cout << "ray_dir: " << glm::to_string(ray_dir) << endl;
	if (isect.t < HUGE_VAL) {
		// get color
		vec3 color = getColor(isect, start, ray_dir, obj->m_material,0,0);
		if (!resample) {
			not_bg_map[(int)(j*w+i)] = true;
		}
		return color;
	} else {
		// get bg color
		unsigned char* pixelOffset = bg_data + (int)(i + bg_x * j) * channels;
		if (!resample) {
			not_bg_map[(int)(j*w+i)] = false;
		}
		return vec3((uint)pixelOffset[0]/255.0,(uint)pixelOffset[1]/255.0,(uint)pixelOffset[2]/255.0);
	}
}
// given pixel location, calculate ray direction and sample the color
glm::vec3 A4::A4_sample_one_pixel(
	double i, double j, bool resample
) {
	vec3 ray_dir = get_ray_dir(i,j);
	vec3 color = vec3(0,0,0);
	vec3 start = eye;
	// color += A4_sample_one_dir(ray_dir, start, i, j, resample);
	if (!DEPTH_OF_FIELD) {
		return A4_sample_one_dir(ray_dir, start, i, j, resample);
	} else {
		vec3 offset_ray_dir;
		double focus_d = (FOCUS - eye[2])/ray_dir[2];
		// cout << "focus_d"
		vec3 dest = eye + focus_d * ray_dir;
		// cout << "dest: " << glm::to_string(dest) << endl;
		for (int i=0; i < DEPTH_OF_FIELD_N; i++) {
			start = eye + randoms[i]*APERTURE_SIZE*vec3(0,1,0) + randoms[i]*APERTURE_SIZE*vec3(1,0,0);
			// cout << "start to eye: " << glm::to_string(start-eye) << endl;
			offset_ray_dir = glm::normalize(dest - start);
			vec3 sample_color = A4_sample_one_dir(offset_ray_dir, start, i, j, true);
			// cout << "sample_color: " << glm::to_string(sample_color) << endl;
			color += sample_color;
		}
		// cout << endl;
		return color / (double)(DEPTH_OF_FIELD_N);
	}
}

bool A4::A4_pixel_is_edge (vec3 avg, vec3 color) {
	bool edge = false;
	for (int k = 0; k < 3; k++) {
		// compute the difference of pixel color to average of neighbors to detect edge
		if (abs(avg[k] - color[k]) > COLOR_THRESHOLD) {
			edge = true;
			break;
		}

	}
	return edge;
}

vec3 A4::get_neighbor_pixel_avg(int i, int j, Image & samples) {
	vec3 avg;
	for (int k = 0; k < 3; k++) {
		avg[k] = 0.25*(samples((uint)i-1, (uint)j, k)+samples((uint)i+1, (uint)j, k)
			+samples((uint)i, (uint)j+1, k)+samples((uint)i, (uint)j-1, k));
	}
	return avg;
}

vec3 A4::recursive_sampling( Image & samples,double i,double j, int level, vec3 avg){
	vec3 color1 = A4_sample_one_pixel((double)i-pow(0.5,level),(double)j-pow(0.5,level),true);
	if (A4_pixel_is_edge(avg, color1) && level < RESAMPLE_LEVEL) {
		color1 = recursive_sampling(samples, (double)i-pow(0.5,level), (double)j-pow(0.5,level), level+1, avg);
	}
	vec3 color2 = A4_sample_one_pixel((double)i-pow(0.5,level),(double)j+pow(0.5,level),true);
	if (A4_pixel_is_edge(avg, color2) & level < RESAMPLE_LEVEL) {
		color2 = recursive_sampling(samples, (double)i-pow(0.5,level), (double)j+pow(0.5,level), level + 1, avg);
	}
	vec3 color3 = A4_sample_one_pixel((double)i+pow(0.5,level),(double)j-pow(0.5,level),true);
	if (A4_pixel_is_edge(avg, color3) & level < RESAMPLE_LEVEL) {
		color3 = recursive_sampling(samples, (double)i+pow(0.5,level), (double)j-pow(0.5,level), level + 1, avg);
	}
	vec3 color4 = A4_sample_one_pixel((double)i+pow(0.5,level),(double)j+pow(0.5,level),true);
	if (A4_pixel_is_edge(avg, color4) & level < RESAMPLE_LEVEL) {
		color4 = recursive_sampling(samples, (double)i+pow(0.5,level), (double)j+pow(0.5,level), level + 1, avg);
	}
	return 0.25 * (color1 + color2 + color3 + color4);
}


// adaptive sample the image
void A4::A4_adaptive_sampling( Image & samples)	{
	image = samples;
	for (int i = 1; i < w -1; i++) {
		for (int j = 1; j < h-1; j++) {
			vec3 color = vec3(samples((uint)i, (uint)j, 0),samples((uint)i, (uint)j, 1),samples((uint)i, (uint)j, 2));
			vec3 avg = get_neighbor_pixel_avg(i, j, samples);
			bool edge = A4_pixel_is_edge (avg, color);
			if (not_bg_map[(int)(j*w+i)] && edge) {
				color = recursive_sampling(samples, (double)i, (double)j, 2, avg);
			}
			for ( int k = 0; k < 3; k++) {
				image((uint)i, (uint)j, k) = color[k];
			}
		}
	}
	// // not performing edge detection on borders. simply copy data
	// for ( int k = 0; k < 3; k++) {
	// 	for (int j = 0; j < h; j++) {
	// 		image(0,j, k) = samples(0,j, k);
	// 		image(w-1,j, k) = samples(w-1,j, k);
	// 	}
	// 	for (int i = 0; i < h; i++) {
	// 		image(i, 0, k) = samples(i, 0, k);
	// 		image(i, h-1, k) = samples(i, h-1, k);
	// 	}
	// }
}

void A4::A4_Render_pixel_rec(
	bool shadow_ray,
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
		Dielectric *dielectric = dynamic_cast<Dielectric *>(gnode->m_material);
		if (dielectric == NULL || !shadow_ray) {
			bool intersect = prim->intersect(vec3(T_new*vec4(start,1.0)),
				vec3(T_new*vec4(ray_dir,0.0)), &tmp_isect);
			if (intersect && tmp_isect.t < isect->t) {
				isect->t = tmp_isect.t;
				isect->normal = vec3(T_n_new*vec4(tmp_isect.normal, 0.0));
				isect->tangent = vec3(glm::inverse(T_new)*vec4(tmp_isect.tangent, 0.0));
				isect->uv = tmp_isect.uv;
				// cout << "isect->uv: " << glm::to_string(isect->uv) << endl;
				*obj = gnode;
			}
		} else {
			// cout << "shadow_ray!! transparent obstacle!!" << endl;
		}
	}
	if (!shadow_ray || isect->t == HUGE_VAL) {
		for (SceneNode *child : root->children) {
			A4::A4_Render_pixel_rec(shadow_ray, child, start, ray_dir, isect, T_new, T_n_new, obj);
		}
	}
}

glm::vec3 A4::readNormalMap(string mapName, vec2 uv) {
	if (normal_maps.find(mapName) == normal_maps.end()) {
		int im_x, im_y, im_c;
		unsigned char *im_data;
		im_data = stbi_load(mapName.c_str(), &im_x, &im_y, &im_c, 0);
		image_data new_normal = image_data{
			im_x,
			im_y,
			im_c,
			im_data
		};
		normal_maps.insert(std::pair<string,image_data>(mapName, new_normal) );
	}
	// texture lookup
	image_data normal_m = normal_maps.at(mapName);
	vec3 normal_shift = 2.0 * normal_m.get_color_at_loc(uv) - vec3(1.0,1.0,1.0);
	return normal_shift;
}

glm::vec3 A4::readTextureMap(string mapName, vec2 uv) {
	if (textures.find(mapName) == textures.end()) {
		int im_x, im_y, im_c;
		unsigned char *im_data;
		im_data = stbi_load(mapName.c_str(), &im_x, &im_y, &im_c, 0);
		image_data new_tex = image_data{
			im_x,
			im_y,
			im_c,
			im_data
		};
		textures.insert(std::pair<string,image_data>(mapName, new_tex) );
	}
	// texture lookup
	image_data tex = textures.at(mapName);
	return tex.get_color_at_loc(uv);
}

glm::vec3 A4::getColor (
	Intersection & isect,
	const vec3 & eye,
	const vec3 & ray_dir,
	Material *material,
	double accum_dist,
	int count
) {
		bool isPhongMat = false;
		PhongMaterial *phongMat = dynamic_cast<PhongMaterial *>(material);
		Dielectric *dielectric = dynamic_cast<Dielectric *>(material);
		Glossy *glossy = dynamic_cast<Glossy *>(material);
		if (phongMat != NULL) {
			isPhongMat = true;
		}
		Intersection tmp_isect1 = Intersection();
		Intersection tmp_isect2 = Intersection();
		vec3 to_light_dir, start;
		vec3 color = vec3(0.0, 0.0, 0.0);
		vec3 intersection = eye + isect.t * ray_dir;
		vec3 n_normal = glm::normalize(isect.normal);
		vec3 n_tangent = glm::normalize(isect.tangent);
		GeometryNode *node;
		vec3 diffuse_color = material->m_kd;
		vec3 specular_color = material->m_ks;

		vec3 diffuse_contrib = vec3(0,0,0);
		vec3 specular_contrib = vec3(0,0,0);
		vec3 ambient_contrib = vec3(0,0,0);
		vec3 reflection_contrib = vec3(0,0,0);
		vec3 transmission_contrib = vec3(0,0,0);

		// cout << "getColor count: " << count << endl;
		// cout << "getColor mat: " << glm::to_string(diffuse_color) << endl;
		// do normal mapping if necessary
		if (isPhongMat && strlen((phongMat->m_normal_fname).c_str()) > 0) {
			// map entry for normal map not yet created
			vec3 normal_shift = readNormalMap(phongMat->m_normal_fname, isect.uv);
			vec3 bitangent = glm::normalize(glm::cross(n_normal, n_tangent));
			vec3 shifted_normal = n_normal + normal_shift[0]*n_tangent + normal_shift[1]*bitangent + normal_shift[2]*n_normal;
			n_normal = glm::normalize(shifted_normal);
		}

		for (Light *light : lights) {
			vec3 to_light = light->position - intersection;
			to_light_dir = glm::normalize(to_light);
			double to_light_dist = glm::length(to_light);
			bool hit_light = false;
			// test if facing light
			double normal_dot_light = glm::dot(to_light_dir, n_normal);
			if (normal_dot_light > 0) {
				// read texture
				if (isPhongMat && strlen((phongMat->m_tex_fname).c_str()) > 0) {
						diffuse_color = readTextureMap(phongMat->m_tex_fname, isect.uv);
						specular_color = diffuse_color;
				}
				start = intersection + EPSILON*to_light_dir;
				// attenuation
				double dist = accum_dist + glm::length(light->position - intersection);
				double atten = 1.0/(light->falloff[0] + light->falloff[1] * dist + light->falloff[2]*pow(dist,2.0));
				double hit_count = 0.0;
				if (SOFT_SHADOW) {
					// light area
					vec3 light_u = glm::normalize(glm::cross(to_light_dir, vec3(0,1.0,0)));
					vec3 light_v = glm::normalize(glm::cross(to_light_dir, light_u));
					for (int i = 0; i < SOFT_SHADOW_N + 1; i++) {
						vec3 light_offset = vec3(0.0,0.0,0.0);
						if (i>0) {
							light_offset = light_u * randoms.at(i-1) * LIGHT_RADIUS + light_v * randomr.at(i-1) * LIGHT_RADIUS;
						}
						tmp_isect1.t = HUGE_VAL;
						A4_Render_pixel_rec (true, root, start, glm::normalize(to_light_dir+light_offset), &tmp_isect1, mat4(1.0), mat4(1.0), &node);
						if (tmp_isect1.t == HUGE_VAL || tmp_isect1.t > to_light_dist){
							hit_count++;
						}
						if (i == 4 && hit_count == 0) {
							break;
						}
					}
					// diffuse
					if (hit_count > 0) {
						diffuse_contrib += light->colour * diffuse_color * atten * normal_dot_light * hit_count / (double)(SOFT_SHADOW_N + 1);
					}
				} else {
					// no soft shadow
					tmp_isect1.t = HUGE_VAL;
					A4_Render_pixel_rec (true, root, start, to_light_dir, &tmp_isect1, mat4(1.0), mat4(1.0), &node);
					if (tmp_isect1.t == HUGE_VAL || tmp_isect1.t > to_light_dist) {
						// diffuse
						diffuse_contrib += light->colour * diffuse_color * normal_dot_light * atten;
						hit_light = true;
					}
				}
				if ((SOFT_SHADOW && hit_count > 0) || hit_light) {
					// Specular
					double cos_thetaI = glm::dot(n_normal, -ray_dir);
					vec3 rf_ray_dir = glm::normalize(2.0*cos_thetaI*n_normal + ray_dir);
					double rf_dot_view = std::fmax(0.0, glm::dot(rf_ray_dir, to_light_dir));
					specular_contrib += specular_color * pow(rf_dot_view, material->m_shininess) * atten;
				}
			}
		}
		ambient_contrib += ambient * diffuse_color;
		int hit_count = 0;
		// if reflextive material
		if (dielectric != NULL && dielectric->m_rf_index >= 1.0f && count <= REFLECT_MAX_TIMES) {
			reflectAndTransmit( n_normal, ray_dir, intersection, diffuse_color,
					&reflection_contrib, &transmission_contrib, *dielectric, accum_dist, count);
		} else if (glossy != NULL && count <= REFLECT_MAX_TIMES) {
			hit_count = glossyReflection(n_normal, ray_dir, intersection, *glossy, accum_dist, count, &reflection_contrib);
		}

		if (dielectric != NULL) {
			return specular_contrib + (reflection_contrib + transmission_contrib);
		} else if (glossy != NULL) {
			double reflection_fraction = hit_count*(GLOSSY_REFL_FRACTION/GLOSSY_REFL_N);
			return specular_contrib + reflection_fraction * reflection_contrib * diffuse_contrib+
				(1.0-reflection_fraction) * diffuse_contrib + ambient_contrib;
		}else {
			return diffuse_contrib + specular_contrib + ambient_contrib;
		}

}

int A4::glossyReflection(
	const vec3 & n_normal,
	const vec3 & ray_dir,
	const vec3 & intersection,
	Glossy & glossy_mat,
	double accum_dist,
	int count,
	vec3 *reflection_contrib
) {
	// cout << "count: " << count << endl;
	vec3 offset, ofs_rf_ray_dir, start_rf;
	GeometryNode *node1;
	Intersection tmp_isect1 = Intersection();
	int hit_count = 0;
	vec3 rf_avg_color = vec3(0,0,0);

	double cos_thetaI = glm::dot(n_normal, -ray_dir);
	vec3 rf_ray_dir = glm::normalize(2.0*cos_thetaI*n_normal + ray_dir);

	vec3 u = glm::normalize(glm::cross(rf_ray_dir, vec3(0,1,0)));
	vec3 v = glm::normalize(glm::cross(rf_ray_dir, u));

	for (int i = 0; i < GLOSSY_REFL_N; i++) {
		if (i > 0) {
			offset = u * (randoms.at(i)-0.5) * glossy_mat.m_gloss_index + v * (randomr.at(i)-0.5) * glossy_mat.m_gloss_index;
		} else {
			offset = vec3(0,0,0);
		}
		ofs_rf_ray_dir = glm::normalize(rf_ray_dir + offset);
		start_rf = intersection + EPSILON*ofs_rf_ray_dir;
		tmp_isect1.t = HUGE_VAL;
		A4_Render_pixel_rec (false, root, start_rf, ofs_rf_ray_dir, &tmp_isect1, mat4(1.0), mat4(1.0), &node1);
		if (tmp_isect1.t < HUGE_VAL) {
			hit_count ++;
			rf_avg_color += getColor (tmp_isect1, start_rf, ofs_rf_ray_dir, node1->m_material, accum_dist+tmp_isect1.t, count+1);
		}
	}
	if (hit_count > 0) {
		*reflection_contrib = rf_avg_color/(double)hit_count;
		// cout <<"reflection_contrib: " << glm::to_string(*reflection_contrib) << endl;
	} else {
		*reflection_contrib = vec3(0,0,0);
	}
	return hit_count;
}
void A4::reflectAndTransmit(
	const vec3 & n_normal,
	const vec3 & ray_dir,
	const vec3 & intersection,
	const vec3 & diffuse_color,
	vec3 *reflection_contrib,
	vec3 *transmission_contrib,
	Dielectric & dielectric,
	double accum_dist,
	int count
) {
	Intersection tmp_isect1 = Intersection();
	tmp_isect1.t = HUGE_VAL;
	Intersection tmp_isect2 = Intersection();
	tmp_isect2.t = HUGE_VAL;
	double cos_thetaI, sin_thetaI, sin_thetaT, cos_thetaT;
	GeometryNode *node1, *node2;
	double eta_I, eta_T;
	vec3 normal;
	double r_avg;
	vec3 rf_ray_dir, start_rf;
	vec3 tr_ray_dir, start_tr;

	// compute reflective ray
	cos_thetaI = glm::dot(n_normal, -ray_dir);
	bool from_inside = cos_thetaI < 0;

	if (from_inside) {
		// cout << "from_inside" << endl;
		eta_T = 1.0;
		eta_I = dielectric.m_rf_index;
		cos_thetaI = - cos_thetaI;
		normal = -n_normal;
	} else {
		eta_I = 1.0;
		eta_T = dielectric.m_rf_index;
		normal = n_normal;
	}

	sin_thetaI = sqrt(1-pow(cos_thetaI, 2.0));
	sin_thetaT = sin_thetaI * eta_I/eta_T;
	cos_thetaT = sqrt(1-pow(sin_thetaT, 2.0));

	if (sin_thetaT >= 1.0) {
		// cout << "total_internal!" << endl;
		r_avg = 1.0;
	} else {
		// compute r_avg
		double r_par = (eta_T * cos_thetaI - eta_I * cos_thetaT)/
			(eta_T * cos_thetaI + eta_I * cos_thetaT);
		double r_per = (eta_I * cos_thetaI - eta_T*cos_thetaT)/
			(eta_I * cos_thetaI + eta_T*cos_thetaT);
		r_avg = (pow(r_par,2.0) + pow(r_per,2.0))/2.0;
		r_avg = std::fmin(std::fmax(r_avg, 0.0), 1.0);
		// transmission
		if (r_avg < 1) {
			// cout << "transmission" << endl;
			tr_ray_dir = glm::normalize(ray_dir + normal*cos_thetaI) * sin_thetaT - glm::normalize(normal*cos_thetaI) * cos_thetaT;
			start_tr = intersection + EPSILON*tr_ray_dir;
			tmp_isect2.t = HUGE_VAL;
			A4_Render_pixel_rec (false, root, start_tr, tr_ray_dir, &tmp_isect2, mat4(1.0), mat4(1.0), &node1);
		}
	}
	// reflection
	if (r_avg > 0) {
		rf_ray_dir = glm::normalize(2.0*cos_thetaI*normal + ray_dir);
		start_rf = intersection + EPSILON*rf_ray_dir;
		A4_Render_pixel_rec (false, root, start_rf, rf_ray_dir, &tmp_isect1, mat4(1.0), mat4(1.0), &node2);
	}
	if (tmp_isect1.t != HUGE_VAL || tmp_isect2.t != HUGE_VAL) {
		if (FRESNEL) {
			vec3 rf_color = vec3(0.0,0.0,0.0);
			vec3 tr_color = vec3(0.0,0.0,0.0);
			if (tmp_isect1.t != HUGE_VAL && r_avg > 0) {
				rf_color = getColor (tmp_isect1, start_rf, rf_ray_dir, node2->m_material, accum_dist+tmp_isect1.t, count+1);
			}
			if (tmp_isect2.t < HUGE_VAL) {
				tr_color = getColor (tmp_isect2, start_tr, tr_ray_dir, node1->m_material, accum_dist+tmp_isect2.t, count+1);
				// cout << "tr_color: " << glm::to_string(tr_color) << endl;
			}
			*reflection_contrib = r_avg * rf_color;
			*transmission_contrib = (1.0 - r_avg) * tr_color;
		}
	}
}
