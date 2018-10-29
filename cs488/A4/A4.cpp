// Fall 2018

#include <glm/ext.hpp>

using namespace glm;
using namespace std;

#include "A4.hpp"
#include "GeometryNode.hpp"
#include "PhongMaterial.hpp"

#define PI 3.14159
#define EPSILON 1.0e-10
static double *depth_buffer;

void A4_Render(
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
) {
	depth_buffer = new double[(int)(image.width()*image.height())];
	for (int i = 0; i < image.width()*image.height(); i++) {
		depth_buffer[i] = HUGE_VAL;
	}
 	A4_Render_rec(root, image, eye, view, up, fovy, ambient, lights);
	delete [] depth_buffer;
}

void A4_Render_rec(
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
) {

  // Fill in raytracing code here...
	if ( root -> m_nodeType == NodeType::GeometryNode ){
		GeometryNode * gnode = static_cast<GeometryNode *>(root);
		Primitive *prim = gnode->m_primitive;
		PhongMaterial * mat = static_cast<PhongMaterial *>(gnode->m_material);
		cout << "gnode->m_material: " << glm::to_string(mat->m_kd) << endl;
		// generate rays
		vec3 right = glm::normalize(glm::cross(view, up));
		vec3 ray_dir = vec3(1.0);
		vec3 normal;
		double w = image.width();
		double h = image.height();
		double t;
		double pixsize_over_d = 2.0 * tan(fovy*PI/(2.0*180.0)) / h;
		// cout << "pixsize_over_d: " << pixsize_over_d << endl;
		for (int i = 0; i < w; i++) {
			for (int j = 0; j < h; j++) {
				ray_dir = ((-w/2.0 + i)*right + (h/2.0 - j)*up) *
					pixsize_over_d + view;
				// cout << glm::to_string(ray_dir) << endl;
				ray_dir = glm::normalize(ray_dir);
				bool intersect = prim->intersect(eye, ray_dir, &t, &normal);
				if (intersect) {
					int index = (j-1)*(int)w+i;
					// check if intersection is in the front
					if (t < depth_buffer[index]) {
						depth_buffer[index] = t;
						// get color
						vec3 intersection = eye + t*ray_dir;
						vec3 color = getColor(root, intersection, normal,
							ambient, lights, ray_dir, gnode->m_material);
						for (int k = 0; k < 3; k++) {
								image((uint)i, (uint)j, k) = color[k];
						}
					}
				}
			}
		}
	}
	for (SceneNode *child : root->children) {
		A4_Render_rec(child, image, eye, view, up, fovy, ambient, lights);
	}
}

bool A4_Render_pixel_rec(
		// What to render
		SceneNode * root,

		// Viewing parameters
		const glm::vec3 & start,
		const glm::vec3 & ray_dir,
		double *t,
		vec3 *n
) {
	if ( root -> m_nodeType == NodeType::GeometryNode ){
		GeometryNode * gnode = static_cast<GeometryNode *>(root);
		Primitive *prim = gnode->m_primitive;
		double t_test;
		bool intersect = prim->intersect(start, ray_dir, &t_test, n);
		if (intersect && t_test < *t) {
			*t = t_test;
		}
		for (SceneNode *child : root->children) {
			intersect = A4_Render_pixel_rec(child, start, ray_dir, t, n);
		}
	}
}

glm::vec3 getColor (
	SceneNode * root,
	const vec3 & intersection,
	const vec3 & normal,
	const glm::vec3 & ambient,
	const std::list<Light *> & lights,
	const vec3 & ray_dir,
	Material *material) {
		PhongMaterial * mat = static_cast<PhongMaterial *>(material);
		vec3 to_light_dir, start, n;
		vec3 color = vec3(0.0, 0.0, 0.0);
		double normal_light_angle, t;
		for (Light *light : lights) {
			to_light_dir = glm::normalize(light->position - intersection);
			// test if facing light
			double normal_dot_light = glm::dot(to_light_dir, normal);
			if (normal_dot_light > 0) {
				// test if light is blocked
				start = intersection + EPSILON * to_light_dir;
				if (!A4_Render_pixel_rec (root, start, to_light_dir, &t, &n)) {
					// cout << "mat color: " << glm::to_string(light->colour) << endl;;
					color += light->colour * mat->m_kd * normal_dot_light;
				} else {
					cout << "in shadow!!" << endl;
				}
			}
		}
		return color;
}
