// Fall 2018

#include "PhongMaterial.hpp"

PhongMaterial::PhongMaterial(
	const glm::vec3& kd, const glm::vec3& ks, double shininess, double rf_index)
	: Material(kd, ks, shininess),
		m_rf_index(rf_index)
{}

PhongMaterial::~PhongMaterial()
{}
