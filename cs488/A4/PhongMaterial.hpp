// Fall 2018

#pragma once

#include "Material.hpp"

class PhongMaterial : public Material {
public:
  PhongMaterial(const glm::vec3& kd, const glm::vec3& ks, double shininess,
    const char * tex_fname, const char * m_normal_fname);
  virtual ~PhongMaterial();

  double m_rf_index;
  std::string m_tex_fname;
  std::string m_normal_fname;
};

class Dielectric : public PhongMaterial {
public:
  Dielectric(const glm::vec3& kd, const glm::vec3& ks, double shininess,
    double rf_index, const char * tex_fname, const char * m_normal_fname);
  virtual ~Dielectric();

  double m_rf_index;
};

class Glossy : public PhongMaterial {
public:
  Glossy(const glm::vec3& kd, const glm::vec3& ks, double shininess,
    double glossy_index, const char * tex_fname, const char * m_normal_fname);
  virtual ~Glossy();

  double m_gloss_index;
};
