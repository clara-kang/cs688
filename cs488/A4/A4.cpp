// Fall 2018

#include <glm/ext.hpp>

using namespace glm;
using namespace std;

#include "A4.hpp"
#include "PhongMaterial.hpp"

#define PI 3.14159
#define EPSILON 1.0e-2

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
 	A4_Render_rec(root, image, eye, view, up, fovy, ambient, lights);
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
		vec3 right = glm::normalize(glm::cross(view, up));
		vec3 ray_dir = vec3(1.0);
		vec3 normal;
		vec3 intersection;
		GeometryNode *obj;
		double w = image.width();
		double h = image.height();
		double t;
		double pixsize_over_d = 2.0 * tan(fovy*PI/(2.0*180.0)) / h;
		for (int i = 0; i < w; i++) {
			for (int j = 0; j < h; j++) {
				ray_dir = ((-w/2.0 + i)*right + (h/2.0 - j)*up) *
					pixsize_over_d + view;
				ray_dir = glm::normalize(ray_dir);
				t = HUGE_VAL;
				A4_Render_pixel_rec (root, eye, ray_dir, &t, &normal, mat4(1.0), mat4(1.0), &intersection, &obj);
				// if intersect object
				if (t < HUGE_VAL) {
						// get color
						vec3 color = getColor(root, eye+t*ray_dir, normal,
							ambient, lights, ray_dir, obj->m_material);
						for (int k = 0; k < 3; k++) {
								image((uint)i, (uint)j, k) = color[k];
						}
				}
			}
		}
}

void A4_Render_pixel_rec(
		// What to render
		SceneNode * root,

		// Viewing parameters
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
		A4_Render_pixel_rec(child, start, ray_dir, t, n, T_new, T_n_new, intersection, obj);
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
		vec3 n_normal = glm::normalize(normal);
		vec3 i;
		double normal_light_angle;
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
					color += light->colour * mat->m_kd * normal_dot_light;
					// Specular
					vec3 half = glm::normalize(to_light_dir - ray_dir);
					double normal_dot_half = glm::dot(n_normal, half);
					color += mat->m_ks * pow(normal_dot_half, mat->m_shininess);
				} else {
					// cout << "t: " << t << endl;
					// cout << "in shadow" << endl;
				}
			}
		}
		color += ambient * mat->m_kd;
		return color;
}
