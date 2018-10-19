// Fall 2018

#pragma once

#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"
#include "cs488-framework/MeshConsolidator.hpp"

#include "SceneNode.hpp"

#include <glm/glm.hpp>
#include <memory>
#include <map>

struct LightSource {
	glm::vec3 position;
	glm::vec3 rgbIntensity;
};

struct JointPointer {
	unsigned int m_nodeId;
	const SceneNode* joint;
};

struct JointTransform {
	glm::mat4 trans_matrix;
	double current_x;
	double current_y;
};



class A3 : public CS488Window {
public:
	A3(const std::string & luaSceneFile);
	virtual ~A3();

protected:
	virtual void init() override;
	virtual void appLogic() override;
	virtual void guiLogic() override;
	virtual void draw() override;
	virtual void cleanup() override;

	//-- Virtual callback methods
	virtual bool cursorEnterWindowEvent(int entered) override;
	virtual bool mouseMoveEvent(double xPos, double yPos) override;
	virtual bool mouseButtonInputEvent(int button, int actions, int mods) override;
	virtual bool mouseScrollEvent(double xOffSet, double yOffSet) override;
	virtual bool windowResizeEvent(int width, int height) override;
	virtual bool keyInputEvent(int key, int action, int mods) override;

	//-- One time initialization methods:
	void processLuaSceneFile(const std::string & filename);
	void createShaderProgram();
	void enableVertexShaderInputSlots();
	void uploadVertexDataToVbos(const MeshConsolidator & meshConsolidator);
	void mapVboDataToVertexShaderInputLocations();
	void initViewMatrix();
	void initLightSources();

	void initPerspectiveMatrix();
	void uploadCommonSceneUniforms();
	void renderSceneGraph(const SceneNode &node);
	void renderSceneGraphRec(const SceneNode & root, const glm::mat4 & modelMatrix,
		const glm::mat4 & scaleMatrix);
	void renderArcCircle();

	glm::mat4 m_perpsective;
	glm::mat4 m_view;

	LightSource m_light;

	//-- GL resources for mesh geometry data:
	GLuint m_vao_meshData;
	GLuint m_vbo_vertexPositions;
	GLuint m_vbo_vertexNormals;
	GLint m_positionAttribLocation;
	GLint m_positionAttribLocation_select;
	GLint m_normalAttribLocation;
	ShaderProgram m_shader;
	ShaderProgram m_shader_select;

	//-- GL resources for trackball circle geometry:
	GLuint m_vbo_arcCircle;
	GLuint m_vao_arcCircle;
	GLint m_arc_positionAttribLocation;
	ShaderProgram m_shader_arcCircle;

	// BatchInfoMap is an associative container that maps a unique MeshId to a BatchInfo
	// object. Each BatchInfo object contains an index offset and the number of indices
	// required to render the mesh with identifier MeshId.
	BatchInfoMap m_batchInfoMap;

	std::string m_luaSceneFile;

	std::shared_ptr<SceneNode> m_rootNode;

	enum MODE { PO, J};
	int m_mode;
	glm::vec2 screen_center;
	glm::vec2 prev_mouse_pos;
	glm::vec3 trackball_o;
	glm::vec3 trackball_u;
	// glm::vec2 released_pos;
	bool mouse_down;
	bool trackball_rotate;
	float aspect;
	int m_button;
	bool m_select_mode;

	glm::mat4 root_rotation, root_translation, root_scale;
	void createColorMap ();
	int getIndexInColorMap (glm::vec3 color);
	void extractRootTransMatrices();
	void selectNodes (int node_index);
	void selectChildrenRec (const SceneNode & root);
	std::list<JointPointer> jointPointers;
	// node id as key, node pointer as value
	std::map<unsigned int, SceneNode *> m_node_lookup;
	// joint node id as key, value is the stack of transformation
	std::map<unsigned int, std::list<JointTransform>> m_jointTransforms;
	// joint node id as key, value is true is joint selected
	std::map<unsigned int, bool> m_joint_affected;
	void initJointPointers(const SceneNode & root);
	void initJointTransformsRec(const SceneNode & root);
	void initNodeLookupRec(SceneNode & root);
	void showUI() ;
	//void initSelectedJointsRec(const SceneNode & root);

	glm::mat4 current_joint_rotation;

};
