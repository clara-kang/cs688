// Fall 2018

#pragma once

#include "Material.hpp"

class PhongMaterial : public Material {
public:
  PhongMaterial(const glm::vec3& kd, const glm::vec3& ks, double shininess, double rf_index);
  virtual ~PhongMaterial();

  double m_rf_index;
};
