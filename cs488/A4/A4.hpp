// Fall 2018

#pragma once

#include <glm/glm.hpp>

#include "SceneNode.hpp"
#include "GeometryNode.hpp"
#include "Light.hpp"
#include "Image.hpp"

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
);

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
);

glm::vec3 getColor (
	SceneNode * root,
	const vec3 & intersection,
	const vec3 & normal,
	const glm::vec3 & ambient,
	const std::list<Light *> & lights,
	const vec3 & ray_dir,
	Material *material);

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
		GeometryNode **obj
);
