// Fall 2018

#pragma once

#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"

#include <glm/glm.hpp>

#include <vector>

// Set a global maximum number of vertices in order to pre-allocate VBO data
// in one shot, rather than reallocating each frame.
const GLsizei kMaxVertices = 1000;


// Convenience class for storing vertex data in CPU memory.
// Data should be copied over to GPU memory via VBO storage before rendering.
class VertexData {
public:
	VertexData();

	std::vector<glm::vec2> positions;
	std::vector<glm::vec3> colours;
	GLuint index;
	GLsizei numVertices;
};


class A2 : public CS488Window {
public:
	A2();
	virtual ~A2();

protected:
	virtual void init() override;
	virtual void appLogic() override;
	virtual void guiLogic() override;
	virtual void draw() override;
	virtual void cleanup() override;

	virtual bool cursorEnterWindowEvent(int entered) override;
	virtual bool mouseMoveEvent(double xPos, double yPos) override;
	virtual bool mouseButtonInputEvent(int button, int actions, int mods) override;
	virtual bool mouseScrollEvent(double xOffSet, double yOffSet) override;
	virtual bool windowResizeEvent(int width, int height) override;
	virtual bool keyInputEvent(int key, int action, int mods) override;

	void createShaderProgram();
	void enableVertexAttribIndices();
	void generateVertexBuffers();
	void mapVboDataToVertexAttributeLocation();
	void uploadVertexDataToVbos();

	void initLineData();

	void setLineColour(const glm::vec3 & colour);
	glm::mat4 createViewMatrix(const glm::vec3 viewDir, const glm::vec3 eyePos);
	glm::mat4 createProjMatrix(float fov, float n, float f);
	glm::mat4 createWinMatrix();
	glm::mat4 createToLocalMatrix ();
	void drawLine (
			const glm::vec2 & v0,
			const glm::vec2 & v1
	);
	void reset();

	enum MODE { M_R, M_T, M_S, V_R, V_T, PE};

	ShaderProgram m_shader;

	GLuint m_vao;            // Vertex Array Object
	GLuint m_vbo_positions;  // Vertex Buffer Object
	GLuint m_vbo_colours;    // Vertex Buffer Object

	VertexData m_vertexData;

	glm::vec3 m_currentLineColour;

	glm::vec4 octahedronVertices[6]; // Vertices of the octahedron
	glm::mat4 M, V, P, W;
	glm::mat4 M_rotation, M_scale, M_translate;
	glm::mat4 V_rotation, V_scale, V_translate;
	glm::mat4 M_to_local, M_to_local_inv;
	glm::vec3 m_view_dir;
	glm::vec3 m_eye_pos;

	glm::vec4 local_x, local_y, local_z, local_origin;

	// Fields related to projection
	float m_fov;
	float m_near, m_far; // near, far plane;

	int m_mode;

	bool m_mouseButtonActive;
	int mouse_button;

	void printMatrix(glm::mat4 m);

};
