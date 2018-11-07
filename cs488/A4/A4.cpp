// Fall 2018

#include <glm/ext.hpp>

using namespace glm;
using namespace std;

#include "A4.hpp"
#include "PhongMaterial.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define PI 3.14159
#define EPSILON 1.0e-2

static const char *BG_FILE_NAME = "bg.jpg";
static const double COLOR_THRESHOLD = 0.01;
static const double REFLECT_MAX_TIMES = 4;
static unsigned char *bg_data;

static const bool ADAPTIVE_SAMPLING = true;
static const bool FRESNEL = false;
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
lights(lights) {
	// load bg image
	bg_data = stbi_load(BG_FILE_NAME, &bg_x, &bg_y, &channels, 0);
	pixsize_over_d = 2.0 * tan(fovy*PI/(2.0*180.0)) / image.height();
	right = glm::normalize(glm::cross(view, up));
	not_bg_map = new bool[(int)(image.width()*image.height())];
	w = image.width();
	h = image.height();
};

void A4::A4_Render(
) {
	// Get primary samples
	Image primary_samples( w, h);

	// generate rays
	vec3 normal, intersection;
	GeometryNode *obj;
	double t;
	for (int i = 0; i < w; i++) {
		for (int j = 0; j < h; j++) {
			vec3 color = A4_sample_one_dir((double)i,(double)j, false);
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

// given pixel location, calculate ray direction and sample the color
glm::vec3 A4::A4_sample_one_dir(
	double i, double j, bool resample
) {
	vec3 ray_dir = get_ray_dir(i,j);
	vec3 normal, intersection;
	GeometryNode *obj;
	double t;
	t = HUGE_VAL;
	A4_Render_pixel_rec (root, eye, ray_dir, &t, &normal, mat4(1.0), mat4(1.0), &intersection, &obj);
	// if intersect object
	if (t < HUGE_VAL) {
		// get color
		vec3 color = getColor(root, eye+t*ray_dir, normal,
			ambient, lights, ray_dir, obj->m_material,0);
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
		// return vec3(0.5,0.5,0.5);
	}
}

bool A4::A4_pixel_is_edge (int i, int j, Image & samples, vec3 color) {
	double w = image.width();
	double h = image.height();
	bool edge = false;
	for (int k = 0; k < 3; k++) {
		// skip if pixel is background
		if (not_bg_map[(int)(j*w+i)] == true) {
			double avg = 0.25*(samples((uint)i-1, (uint)j, k)+samples((uint)i+1, (uint)j, k)
				+samples((uint)i, (uint)j+1, k)+samples((uint)i, (uint)j-1, k));
			// compute the difference of pixel color to average of neighbors to detect edge
			if (abs(avg - color[k]) > COLOR_THRESHOLD) {
				edge = true;
				break;
			}
		}
	}
	return edge;
}

// adaptive sample the image
void A4::A4_adaptive_sampling( Image & samples)	{
	double w = image.width();
	double h = image.height();
	image = samples;
	for (int i = 1; i < w -1; i++) {
		for (int j = 1; j < h-1; j++) {
			vec3 color = vec3(samples((uint)i, (uint)j, 0),samples((uint)i, (uint)j, 1),samples((uint)i, (uint)j, 2));
			bool edge = A4_pixel_is_edge (i, j, samples, color);
			if (edge) {
				color = 0.25*(A4_sample_one_dir((double)i-0.25,(double)j-0.25,true) + A4_sample_one_dir((double)i-0.25,(double)j+0.25, true)
					+ A4_sample_one_dir((double)i+0.25,(double)j-0.25, true)+A4_sample_one_dir((double)i+0.25,(double)j-0.25, true));
				// color = vec3(1.0,1.0,1.0);
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
	SceneNode *root,
	const glm::vec3 & start,
	const glm::vec3 & ray_dir,
	double *t,
	vec3 *n,
	mat4 T,
	mat4 T_n,
	vec3 *intersection,
	GeometryNode ** obj
) {
	mat4 T_new = root->invtrans * T;
	mat4 T_n_new = T_n * glm::transpose(root->invtrans);
	if ( root -> m_nodeType == NodeType::GeometryNode ){
		GeometryNode * gnode = static_cast<GeometryNode *>(root);
		Primitive *prim = gnode->m_primitive;
		double t_test = HUGE_VAL;
		vec3 normal;
		bool intersect = prim->intersect(vec3(T_new*vec4(start,1.0)),
			vec3(T_new*vec4(ray_dir,0.0)), &t_test, &normal);
		if (intersect && t_test < *t) {
			*t = t_test;
			*obj = gnode;
			*n = vec3(T_n_new*vec4(normal, 0.0));
			*intersection = start + t_test*ray_dir;
		}
	}
	for (SceneNode *child : root->children) {
		A4::A4_Render_pixel_rec(child, start, ray_dir, t, n, T_new, T_n_new, intersection, obj);
	}
}

glm::vec3 A4::getColor (
	SceneNode *root,
	const vec3 & intersection,
	const vec3 & normal,
	const glm::vec3 & ambient,
	const std::list<Light *> & lights,
	const vec3 & ray_dir,
	Material *material,
	int count
) {
		bool isPhongMat = false;
		PhongMaterial *phongMat = dynamic_cast<PhongMaterial *>(material);
		if (phongMat != NULL) {
			isPhongMat = true;
		}
		vec3 to_light_dir, start, n;
		vec3 color = vec3(0.0, 0.0, 0.0);
		vec3 n_normal = glm::normalize(normal);
		vec3 i; // intersection
		double t = HUGE_VAL;
		GeometryNode *node;
		for (Light *light : lights) {
			to_light_dir = glm::normalize(light->position - intersection);
			// test if facing light
			double normal_dot_light = glm::dot(to_light_dir, n_normal);
			if (normal_dot_light > 0) {
				// test if light is blocked
				start = intersection + EPSILON*to_light_dir;
				t = HUGE_VAL;
				A4_Render_pixel_rec (root, start, to_light_dir, &t, &n, mat4(1.0), mat4(1.0), &i, &node);

				if (t == HUGE_VAL) {
					// diffuse
					color += light->colour * material->m_kd * normal_dot_light;
					// Specular
					double cos_thetaI = glm::dot(n_normal, -ray_dir);
					vec3 rf_ray_dir = glm::normalize(2.0*cos_thetaI*n_normal + ray_dir);
					double rf_dot_view = std::fmax(0.0, glm::dot(rf_ray_dir, to_light_dir));
					color += material->m_ks * pow(rf_dot_view, material->m_shininess);
				}
			}
		}
		color += ambient * material->m_kd;
		if (isPhongMat && count <= REFLECT_MAX_TIMES) {
			// compute reflective ray
			double cos_thetaI = glm::dot(n_normal, -ray_dir);
			vec3 rf_ray_dir = glm::normalize(2.0*cos_thetaI*n_normal + ray_dir);
			start = intersection + EPSILON*rf_ray_dir;
			t = HUGE_VAL;
			A4_Render_pixel_rec (root, start, rf_ray_dir, &t, &n, mat4(1.0), mat4(1.0), &i, &node);
			if (t != HUGE_VAL) {
				if (FRESNEL) {
					double sin_thetaI = sqrt(1-pow(cos_thetaI, 2.0));
					double sin_thetaT = sin_thetaI/phongMat->m_rf_index;
					double r_avg;
					if (sin_thetaT >= 1) {
						r_avg = 1;
					} else {
						double cos_thetaT = sqrt(1-pow(sin_thetaT, 2.0));
						double r_par = (phongMat->m_rf_index * cos_thetaI - cos_thetaT)/
							(phongMat->m_rf_index * cos_thetaI + cos_thetaT);
						double r_per = (cos_thetaI - phongMat->m_rf_index*cos_thetaT)/
							(cos_thetaI + phongMat->m_rf_index*cos_thetaT);
						r_avg = (pow(r_par,2.0) + pow(r_per,2.0))/2.0;
					}
					if (r_avg > 0.0) {
						// cout << "r_avg: " << r_avg << endl;
						vec3 rf_color = getColor (root, i, n, ambient, lights, rf_ray_dir, node->m_material, count+1);
						color = r_avg * rf_color + (1.0 - r_avg) * color;
					}
				} else {
					vec3 rf_color = getColor (root, i, n, ambient, lights, rf_ray_dir, node->m_material, count+1);
					color = 0.3 * rf_color + (1.0 - 0.3) * color;
				}
			}
		}
		return color;
}
