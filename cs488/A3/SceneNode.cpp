// Fall 2018

#include "SceneNode.hpp"

#include "cs488-framework/MathUtils.hpp"

#include <iostream>
#include <sstream>
using namespace std;

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/string_cast.hpp>

using namespace glm;


// Static class variable
unsigned int SceneNode::nodeInstanceCount = 0;


//---------------------------------------------------------------------------------------
SceneNode::SceneNode(const std::string& name)
  : m_name(name),
	m_nodeType(NodeType::SceneNode),
	trans(mat4()),
  m_rotation(mat4(1.0f)),
  m_translation(mat4(1.0f)),
  m_scale(mat4(1.0f)),
	isSelected(false),
	m_nodeId(nodeInstanceCount++)
{

}

//---------------------------------------------------------------------------------------
// Deep copy
SceneNode::SceneNode(const SceneNode & other)
	: m_nodeType(other.m_nodeType),
	  m_name(other.m_name),
	  trans(other.trans),
	  invtrans(other.invtrans)
{
	for(SceneNode * child : other.children) {
		this->children.push_front(new SceneNode(*child));
	}
}

//---------------------------------------------------------------------------------------
SceneNode::~SceneNode() {
	for(SceneNode * child : children) {
		delete child;
	}
}

//---------------------------------------------------------------------------------------
void SceneNode::set_transform(const glm::mat4& m) {
	trans = m;
	invtrans = m;
}

//---------------------------------------------------------------------------------------
const glm::mat4& SceneNode::get_transform() const {
	return trans;
}

//---------------------------------------------------------------------------------------
const glm::mat4& SceneNode::get_inverse() const {
	return invtrans;
}

//---------------------------------------------------------------------------------------
void SceneNode::add_child(SceneNode* child) {
	children.push_back(child);
}

//---------------------------------------------------------------------------------------
void SceneNode::remove_child(SceneNode* child) {
	children.remove(child);
}

//---------------------------------------------------------------------------------------
void SceneNode::rotate(char axis, float angle) {
	vec3 rot_axis;

	switch (axis) {
		case 'x':
			rot_axis = vec3(1,0,0);
			break;
		case 'y':
			rot_axis = vec3(0,1,0);
	        break;
		case 'z':
			rot_axis = vec3(0,0,1);
	        break;
		default:
			break;
	}
	mat4 rot_matrix = glm::rotate(degreesToRadians(angle), rot_axis);
  //m_rotation = rot_matrix * m_rotation;
	trans = rot_matrix * trans;
  extractRootTRSMatrices(trans);
}

//---------------------------------------------------------------------------------------
void SceneNode::scale(const glm::vec3 & amount) {
	trans = glm::scale(amount) * trans;
  //m_scale = glm::scale(amount) * m_scale;
  extractRootTRSMatrices(trans);
}

//---------------------------------------------------------------------------------------
void SceneNode::translate(const glm::vec3& amount) {
	trans = glm::translate(amount) * trans;
  // m_translation = glm::translate(amount) * m_translation;
  // cout << "trans: " << glm::to_string(trans) << endl;
  extractRootTRSMatrices(trans);
}

void SceneNode::extractRootTRSMatrices(const glm::mat4& mat) {
	vec3 mat_s_extract = vec3(glm::length(mat[0]), glm::length(mat[1]),	glm::length(mat[2]));
	m_scale = mat4(1.0f);
	for (int i = 0; i < 3; i++) {
		m_scale[i][i] = mat_s_extract[i];
	}
	mat3 mat_r_extract = mat3(mat);
	for (int i = 0; i < 3; i++) {
		mat_r_extract[i] = mat_r_extract[i] / mat_s_extract[i];
	}
	m_rotation = mat4(vec4(mat_r_extract[0], 0.0f), vec4(mat_r_extract[1], 0.0f),
		vec4(mat_r_extract[2], 0.0f), vec4(0.0f, 0.0f, 0.0f, 1.0f));
	vec3 mat_t_extract = vec3(mat[3]);
	m_translation = glm::translate(mat4(1.0f), mat_t_extract);
  //cout << "m_translation: " << m_translation << endl;
  // cout << "m_rotation: " << m_rotation << endl;
  //cout << "m_scale: " << m_scale << endl;
}

//---------------------------------------------------------------------------------------
int SceneNode::totalSceneNodes() const {
	return nodeInstanceCount;
}

//---------------------------------------------------------------------------------------
std::ostream & operator << (std::ostream & os, const SceneNode & node) {

	//os << "SceneNode:[NodeType: ___, name: ____, id: ____, isSelected: ____, transform: ____"
	switch (node.m_nodeType) {
		case NodeType::SceneNode:
			os << "SceneNode";
			break;
		case NodeType::GeometryNode:
			os << "GeometryNode";
			break;
		case NodeType::JointNode:
			os << "JointNode";
			break;
	}
	os << ":[";

	os << "name:" << node.m_name << ", ";
	os << "id:" << node.m_nodeId;
	os << "]";

	return os;
}
