// Fall 2018

#pragma once

#include <glm/glm.hpp>

class Material {
public:
  virtual ~Material();

// protected:
  //Material();
	Material(glm::vec3 kd, glm::vec3 ks,double shininess);
	glm::vec3 m_kd;
	glm::vec3 m_ks;
	double m_shininess;
};
