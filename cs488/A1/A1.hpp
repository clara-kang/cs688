// Fall 2018

#pragma once

#include <glm/glm.hpp>

#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"

//#include "grid.hpp"
#include "maze.hpp"

class A1 : public CS488Window {
public:
	A1();
	virtual ~A1();

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

private:
	void initGrid();
	void newMaze();
	bool outOfMaze(int x, int y);
	void updateViewMatrix(float zoomFactor);
	void reset();
	int getStartPosY();

	// Fields related to the shader and uniforms.
	ShaderProgram m_shader;
	GLint P_uni; // Uniform location for Projection matrix.
	GLint V_uni; // Uniform location for View matrix.
	GLint M_uni; // Uniform location for Model matrix.
	GLint col_uni;   // Uniform location for cube colour.
	GLint light_uni; // Uniform location for light direction

	// Fields related to grid geometry.
	GLuint m_grid_vao; // Vertex Array Object
	GLuint m_grid_vbo; // Vertex Buffer Object

	// Fields related to floor geometry
	GLuint m_floor_vao;
	GLuint m_floor_vbo;

	//Fields related to cubes geometry
	GLuint m_cube_vao;
	GLuint m_cube_vbo;
	glm::mat4 M_Cube_Scale;
	glm::mat4 M_Avatar_Scale;

	// Matrices controlling the camera and projection.
	glm::mat4 proj;
	glm::mat4 view;

	// Matrix for grid rotation
	glm::mat4 grid_rotation;

	float colour[3];
	// Color for geometries
	glm::vec3 floor_col;
	glm::vec3 avatar_col;
	glm::vec3 cube_col;

	int current_col;
	// Zoom factor, smaller when camera is closer to the object
	float zoom_factor;
	// Persistent rotation direction, -1 cw, 1 ccw, 0 not rotating
	float persistent_rotation_dir;
	// Avatar position
	int avatar_pos[2];
	Maze m;

	// True if the first maze is digged
	bool maze_created;

	// True if shift key is down
	bool shift_down;
	// True if mouse button is currently down.
	bool m_mouseButtonActive;

	//Light direction
	glm::vec3 light_dir;
};
