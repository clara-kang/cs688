// Fall 2018

#include "Material.hpp"

// Material::Material()
// {}

Material::Material(glm::vec3 kd, glm::vec3 ks,double shininess):
  m_kd(kd),
  m_ks(ks),
  m_shininess(shininess){

}

Material::~Material()
{}
