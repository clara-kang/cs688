// Fall 2018

#include <string>
#include <iostream>

#include "PhongMaterial.hpp"
using namespace std;
PhongMaterial::PhongMaterial(
	const glm::vec3& kd, const glm::vec3& ks, double shininess,
	const char *tex_fname, const char * m_normal_fname)
	: Material(kd, ks, shininess),
		m_tex_fname(tex_fname),
		m_normal_fname(m_normal_fname)
{
	// cout << "m_tex_fname: " << m_tex_fname << endl;
}

PhongMaterial::~PhongMaterial()
{}

Dielectric::Dielectric (
	const glm::vec3& kd, const glm::vec3& ks, double shininess,
  double rf_index, const char * tex_fname, const char * normal_fname)
	: PhongMaterial(kd, ks, shininess, tex_fname, normal_fname),
		m_rf_index(rf_index)
		{

		}
Dielectric::~Dielectric(){}

Glossy::Glossy (
	const glm::vec3& kd, const glm::vec3& ks, double shininess,
  double glossy_index, const char * tex_fname, const char * normal_fname)
	: PhongMaterial(kd, ks, shininess, tex_fname, normal_fname),
		m_gloss_index(glossy_index)
		{

		}
Glossy::~Glossy(){}
