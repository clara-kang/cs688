// Fall 2018

#include <string>
#include <iostream>

#include "PhongMaterial.hpp"
using namespace std;
PhongMaterial::PhongMaterial(
	const glm::vec3& kd, const glm::vec3& ks, double shininess, double rf_index, const char *tex_fname)
	: Material(kd, ks, shininess),
		m_rf_index(rf_index),
		m_tex_fname(tex_fname)
{
	cout << "m_tex_fname: " << m_tex_fname << endl;
}

PhongMaterial::~PhongMaterial()
{}
