// Fall 2018

#include "A2.hpp"
#include "cs488-framework/GlErrorCheck.hpp"

#include <iostream>
using namespace std;

#include <imgui/imgui.h>
#include <tgmath.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
// todo: Remove
#include <glm/gtx/string_cast.hpp>
using namespace glm;

#define PI 3.1415
const float MIN_SCALE = 0.1f;
const float INPUT_SCALE_FACTOR = 0.001f;
const float MARGIN = 0.05;
const vec4 X_VECTOR = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
const vec4 Y_VECTOR = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
const vec4 Z_VECTOR = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
const vec4 ORIGIN = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
const vec4 EYE = glm::vec4(0.0f, 0.0f, 15.0f, 1.0f);

//----------------------------------------------------------------------------------------
// Constructor
VertexData::VertexData()
	: numVertices(0),
	  index(0)
{
	positions.resize(kMaxVertices);
	colours.resize(kMaxVertices);
}


//----------------------------------------------------------------------------------------
// Constructor
A2::A2()
	:	window_width(768),
		window_height(768),
		changing_vp(false)
{
	reset();
}

//----------------------------------------------------------------------------------------
// Destructor
A2::~A2()
{

}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A2::init()
{
	// Set the background colour.
	glClearColor(0.3, 0.5, 0.7, 1.0);

	createShaderProgram();

	glGenVertexArrays(1, &m_vao);

	enableVertexAttribIndices();

	generateVertexBuffers();

	mapVboDataToVertexAttributeLocation();
	W = createWinMatrix(vec2(-(1-MARGIN), (1-MARGIN)),
		vec2((1-MARGIN), -(1-MARGIN)));
}

//----------------------------------------------------------------------------------------
void A2::createShaderProgram()
{
	m_shader.generateProgramObject();
	m_shader.attachVertexShader( getAssetFilePath("VertexShader.vs").c_str() );
	m_shader.attachFragmentShader( getAssetFilePath("FragmentShader.fs").c_str() );
	m_shader.link();
}

//---------------------------------------------------------------------------------------- Fall 2018
void A2::enableVertexAttribIndices()
{
	glBindVertexArray(m_vao);

	// Enable the attribute index location for "position" when rendering.
	GLint positionAttribLocation = m_shader.getAttribLocation( "position" );
	glEnableVertexAttribArray(positionAttribLocation);

	// Enable the attribute index location for "colour" when rendering.
	GLint colourAttribLocation = m_shader.getAttribLocation( "colour" );
	glEnableVertexAttribArray(colourAttribLocation);

	// Restore defaults
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void A2::generateVertexBuffers()
{
	// Generate a vertex buffer to store line vertex positions
	{
		glGenBuffers(1, &m_vbo_positions);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_positions);

		// Set to GL_DYNAMIC_DRAW because the data store will be modified frequently.
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * kMaxVertices, nullptr,
				GL_DYNAMIC_DRAW);


		// Unbind the target GL_ARRAY_BUFFER, now that we are finished using it.
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CHECK_GL_ERRORS;
	}

	// Generate a vertex buffer to store line colors
	{
		glGenBuffers(1, &m_vbo_colours);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_colours);

		// Set to GL_DYNAMIC_DRAW because the data store will be modified frequently.
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * kMaxVertices, nullptr,
				GL_DYNAMIC_DRAW);


		// Unbind the target GL_ARRAY_BUFFER, now that we are finished using it.
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CHECK_GL_ERRORS;
	}
}

//----------------------------------------------------------------------------------------
void A2::mapVboDataToVertexAttributeLocation()
{
	// Bind VAO in order to record the data mapping.
	glBindVertexArray(m_vao);

	// Tell GL how to map data from the vertex buffer "m_vbo_positions" into the
	// "position" vertex attribute index for any bound shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_positions);
	GLint positionAttribLocation = m_shader.getAttribLocation( "position" );
	glVertexAttribPointer(positionAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	// Tell GL how to map data from the vertex buffer "m_vbo_colours" into the
	// "colour" vertex attribute index for any bound shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_colours);
	GLint colorAttribLocation = m_shader.getAttribLocation( "colour" );
	glVertexAttribPointer(colorAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//---------------------------------------------------------------------------------------
void A2::initLineData()
{
	m_vertexData.numVertices = 0;
	m_vertexData.index = 0;
}

//---------------------------------------------------------------------------------------
void A2::setLineColour (
		const glm::vec3 & colour
) {
	m_currentLineColour = colour;
}

//---------------------------------------------------------------------------------------
void A2::drawLine(
		const glm::vec2 & V0,   // Line Start (NDC coordinate)
		const glm::vec2 & V1    // Line End (NDC coordinate)
) {

	m_vertexData.positions[m_vertexData.index] = V0;
	m_vertexData.colours[m_vertexData.index] = m_currentLineColour;
	++m_vertexData.index;
	m_vertexData.positions[m_vertexData.index] = V1;
	m_vertexData.colours[m_vertexData.index] = m_currentLineColour;
	++m_vertexData.index;

	m_vertexData.numVertices += 2;
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void A2::appLogic()
{
	// Call at the beginning of frame, before drawing lines:
	initLineData();
	// Transform view basis
	V = createToLocalMatrix(view_x, view_y, view_z,eye);
	mat4 V_inv = glm::inverse(V);
	view_x = V_inv * V_rotation * V * view_x;
	view_y = V_inv * V_rotation * V * view_y;
	view_z = V_inv * V_rotation * V * view_z;
	eye = V_inv * V_translate * V * eye;

	// Transform world gnomons
	vec4 X_VECTOR_pv = W * P * V * X_VECTOR;
	vec4 Y_VECTOR_pv = W * P * V * Y_VECTOR;
	vec4 Z_VECTOR_pv = W * P * V * Z_VECTOR;
	vec4 origin_pv = W * P * V * ORIGIN;

	vec4 x_endpoint = origin_pv + X_VECTOR_pv;
	vec4 y_endpoint = origin_pv + Y_VECTOR_pv;
	vec4 z_endpoint = origin_pv + Z_VECTOR_pv;
	// Draw world gnomons
	setLineColour(vec3(1.0f, 1.0f, 0.0f));
	drawLine(vec2(origin_pv/origin_pv[3]), vec2(x_endpoint/x_endpoint[3]));
	setLineColour(vec3(0.0f, 1.0f, 1.0f));
	drawLine(vec2(origin_pv/origin_pv[3]), vec2(y_endpoint/y_endpoint[3]));
	setLineColour(vec3(1.0f, 0.0f, 1.0f));
	drawLine(vec2(origin_pv/origin_pv[3]), vec2(z_endpoint/z_endpoint[3]));

	// Calculate model transformation matrix, and the matrix to change to model coordinate
	M = M_translate * M_rotation * M_scale;
	M_to_local = createToLocalMatrix(local_x, local_y, local_z, local_o);
	M_to_local_inv = glm::inverse(M_to_local);

	// Transform model gnomons
	local_x = M_to_local_inv * M_rotation * M_to_local * local_x;
  local_y = M_to_local_inv * M_rotation * M_to_local * local_y;
  local_z = M_to_local_inv * M_rotation * M_to_local * local_z;
	local_o = M_to_local_inv * M_translate * M_to_local * local_o;

	// Transform local gnomons to screen space
	vec4 local_o_trans = W * P * V * local_o;
	vec4 local_x_trans = W * P * V * local_x + local_o_trans;
	vec4 local_y_trans = W * P * V * local_y + local_o_trans;
	vec4 local_z_trans = W * P * V * local_z + local_o_trans;
	// Draw local gnomons
	setLineColour(vec3(1.0f, 0.0f, 0.0f));
	drawLine(vec2(local_o_trans/local_o_trans[3]), vec2(local_x_trans/local_x_trans[3]));
	setLineColour(vec3(0.0f, 1.0f, 0.0f));
	drawLine(vec2(local_o_trans/local_o_trans[3]), vec2(local_y_trans/local_y_trans[3]));
	setLineColour(vec3(0.0f, 0.0f, 1.0f));
	drawLine(vec2(local_o_trans/local_o_trans[3]), vec2(local_z_trans/local_z_trans[3]));

	// Transform octahedron vertices
	vec4 vertices_trans[6]; // vertices in world space
	vec2 points[6]; // vertices in screen spcace
	for ( int i = 0; i < 6; i++ ) {
		octahedronVertices[i] = M_to_local_inv * M  * M_to_local * octahedronVertices[i];
		vertices_trans[i] = W * P * V  * octahedronVertices[i];
		points[i] = vec2(vertices_trans[i]/vertices_trans[i][3]);
	}
	// Draw octahedron
	setLineColour(vec3(1.0f, 1.0f, 1.0f));
	drawLine(points[1], points[4]);
	drawLine(points[4], points[0]);
	drawLine(points[0], points[5]);
	drawLine(points[5], points[1]);
	drawLine(points[0], points[2]);
	drawLine(points[1], points[2]);
	drawLine(points[4], points[2]);
	drawLine(points[5], points[2]);
	drawLine(points[0], points[3]);
	drawLine(points[1], points[3]);
	drawLine(points[4], points[3]);
	drawLine(points[5], points[3]);

	// Draw Viewport
	drawLine(vp_tl, vec2(vp_br[0], vp_tl[1]));
	drawLine(vec2(vp_br[0], vp_tl[1]), vp_br);
	drawLine(vp_br, vec2(vp_tl[0], vp_br[1]));
	drawLine(vec2(vp_tl[0], vp_br[1]), vp_tl);

}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A2::guiLogic()
{
	static bool firstRun(true);
	if (firstRun) {
		ImGui::SetNextWindowPos(ImVec2(50, 50));
		firstRun = false;
	}

	static bool showDebugWindow(true);
	ImGuiWindowFlags windowFlags(ImGuiWindowFlags_AlwaysAutoResize);
	float opacity(0.5f);

	ImGui::Begin("Properties", &showDebugWindow, ImVec2(100,100), opacity,
			windowFlags);

	if( ImGui::Button( "Quit Application (Q)" ) ) {
		glfwSetWindowShouldClose(m_window, GL_TRUE);
	}
	if( ImGui::Button( "Reset (R)" ) ) {
		reset();
	}

	ImGui::RadioButton( "Rotate model (R)", &m_mode, M_R );
	ImGui::RadioButton( "Scale model (S)", &m_mode, M_S );
	ImGui::RadioButton( "Translate model (T)", &m_mode, M_T );
	ImGui::RadioButton( "Rotate view (O)", &m_mode, V_R );
	ImGui::RadioButton( "Translate view (E)", &m_mode, V_T );
	ImGui::RadioButton( "Perspective (P)", &m_mode, PE );
	ImGui::RadioButton( "Viewport (V)", &m_mode, VP );

	ImGui::Text( "Framerate: %.1f FPS", ImGui::GetIO().Framerate );

	ImGui::End();
}

//----------------------------------------------------------------------------------------
void A2::uploadVertexDataToVbos() {

	//-- Copy vertex position data into VBO, m_vbo_positions:
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_positions);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec2) * m_vertexData.numVertices,
				m_vertexData.positions.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CHECK_GL_ERRORS;
	}

	//-- Copy vertex colour data into VBO, m_vbo_colours:
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_colours);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec3) * m_vertexData.numVertices,
				m_vertexData.colours.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CHECK_GL_ERRORS;
	}
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A2::draw()
{
	uploadVertexDataToVbos();

	glBindVertexArray(m_vao);

	m_shader.enable();
		glDrawArrays(GL_LINES, 0, m_vertexData.numVertices);
	m_shader.disable();

	// Restore defaults
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A2::cleanup()
{

}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A2::cursorEnterWindowEvent (
		int entered
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse cursor movement events.
 */
bool A2::mouseMoveEvent (
		double xPos,
		double yPos
) {
	bool eventHandled(false);

	if ( m_mouseButtonActive ) {
		switch ( m_mode ) {
			case M_R : {
				float angle = ImGui::GetIO().MouseDelta.x * INPUT_SCALE_FACTOR;
				mat4 D_M_rotation = mat4(1.0f);
				switch ( mouse_button ) {
					case 0: {// left mouse button, rotate model about x axis
						D_M_rotation[1] = vec4(0.0f, cos(angle), sin(angle), 0.0f);
						D_M_rotation[2] = vec4(0.0f, -sin(angle), cos(angle), 0.0f);
						break;
					}
					case 1: {// right mouse button, rotate model about z axis
						D_M_rotation[0] = vec4(cos(angle), sin(angle), 0.0f, 0.0f);
						D_M_rotation[1] = vec4(-sin(angle), cos(angle), 0.0f, 0.0f);
						break;
					}
					case 2: {// middle mouse button, rotate model about y axis
						D_M_rotation[0] = vec4(cos(angle), 0.0f, -sin(angle), 0.0f);
						D_M_rotation[2] = vec4(sin(angle), 0.0f, cos(angle), 0.0f);
						break;
					}
				}
				M_rotation = D_M_rotation;
				break;
			}
			case M_S: {
				float d_scale = ImGui::GetIO().MouseDelta.x * INPUT_SCALE_FACTOR ;
				M_scale = mat4(1.0f);
				switch ( mouse_button ) {
					case 0: {// left mouse button, scale model about x axis
						float d_x = std::max(M_scale[0][0] + d_scale, MIN_SCALE);
						M_scale[0][0] = d_x;
						break;
					}
					case 1: {// right mouse button, scale model about z axis
						float d_z = std::max(M_scale[2][2] + d_scale, MIN_SCALE);
						M_scale[2][2] = d_z;
						break;
					}
					case 2: {// middle mouse button, scale model about y axis
						float d_y = std::max(M_scale[1][1] + d_scale, MIN_SCALE);
						M_scale[1][1] = d_y;
						break;
					}
				}
				break;
			}
			case M_T: {
				float d_translate = ImGui::GetIO().MouseDelta.x * INPUT_SCALE_FACTOR ;
				M_translate = mat4(1.0f);
				switch ( mouse_button ) {
					case 0: {// left mouse button, translate model about x axis
						M_translate[3][0] = d_translate;
						break;
					}
					case 1: {// right mouse button, translate model about z axis
						M_translate[3][2] = d_translate;
						break;
					}
					case 2: {// middle mouse button, translate model about y axis
						M_translate[3][1] = d_translate;
						break;
					}
				}
				break;
			}
			case V_R: {
				float angle = ImGui::GetIO().MouseDelta.x * INPUT_SCALE_FACTOR;
				mat4 D_V_rotation = mat4(1.0f);
				switch ( mouse_button ) {
					case 0: {// left mouse button, rotate view vector about x axis
						D_V_rotation[1] = vec4(0.0f, cos(angle), sin(angle), 0.0f);
						D_V_rotation[2] = vec4(0.0f, -sin(angle), cos(angle), 0.0f);
						break;
					}
					case 1: {// right mouse button, rotate view vector about z axis
						D_V_rotation[0] = vec4(cos(angle), sin(angle), 0.0f, 0.0f);
						D_V_rotation[1] = vec4(-sin(angle), cos(angle), 0.0f, 0.0f);
						break;
					}
					case 2: {// middle mouse button, rotate view vector about y axis
						D_V_rotation[0] = vec4(cos(angle), 0.0f, -sin(angle), 0.0f);
						D_V_rotation[2] = vec4(sin(angle), 0.0f, cos(angle), 0.0f);
						break;
					}
				}
				V_rotation = D_V_rotation;
				break;
			}
			case V_T: {
				float d_translate = ImGui::GetIO().MouseDelta.x * INPUT_SCALE_FACTOR ;
				V_translate = mat4(1.0f);
				switch ( mouse_button ) {
					case 0: {// left mouse button, translate view about x axis
						V_translate[3][0] = d_translate;
						break;
					}
					case 1: {// right mouse button, translate view about z axis
						V_translate[3][2] = d_translate;
						break;
					}
					case 2: {// middle mouse button, translate view about y axis
						V_translate[3][1] = d_translate;
						break;
					}
				}
				break;
			}
			case PE: {
				switch ( mouse_button ) {
					case 0: {// left mouse button, rotate model about x axis
					}
					case 1: {// right mouse button, rotate model about z axis
					}
					case 2: {// middle mouse button, rotate model about y axis
					}
				}
				break;
			}
			case VP: {
				vp_br = vec2(ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y);
				vp_br[0] = vp_br[0] * 2.0f / window_width - 1.0f;
				vp_br[1] = 1.0f - vp_br[1] * 2.0f / window_height;
				W = createWinMatrix(vp_tl, vp_br);
			}
		}
	}

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool A2::mouseButtonInputEvent (
		int button,
		int actions,
		int mods
) {
	bool eventHandled(false);
	if ( actions == GLFW_PRESS ) {
		if (!ImGui::IsMouseHoveringAnyWindow()) {
			m_mouseButtonActive = true;
			mouse_button = button;
			if (m_mode == VP) {
				vp_tl = vec2(ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y);
				vp_tl[0] = vp_tl[0] * 2.0f / window_width - 1.0f;
				vp_tl[1] = 1.0f - vp_tl[1] * 2.0f / window_height;
				vp_br = vp_tl;
				W = createWinMatrix(vp_tl, vp_br);
				changing_vp = true;
			}
		}
	}
	if ( actions == GLFW_RELEASE ) {
		m_mouseButtonActive = false;
		M_rotation = mat4(1.0f);
		M_scale = mat4(1.0f);
		M_translate = mat4(1.0f);
		V_rotation = mat4(1.0f);
		V_translate = mat4(1.0f);
		if (changing_vp) {
			vp_br = vec2(ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y);
			vp_br[0] = vp_br[0] * 2.0f / window_width - 1.0f;
			vp_br[1] = 1.0f - vp_br[1] * 2.0f / window_height;
			W = createWinMatrix(vp_tl, vp_br);
			changing_vp = false;
		}
	}
	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool A2::mouseScrollEvent (
		double xOffSet,
		double yOffSet
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles window resize events.
 */
bool A2::windowResizeEvent (
		int width,
		int height
) {
	bool eventHandled(false);

	// Fill in with event handling code...
	window_width = width;
	window_height = height;
	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool A2::keyInputEvent (
		int key,
		int action,
		int mods
) {
	bool eventHandled(false);

	switch (key) {
		case GLFW_KEY_Q: {
			glfwSetWindowShouldClose(m_window, GL_TRUE);
			eventHandled = true;
			break;
		}
		case GLFW_KEY_A: {
			reset ();
			break;
		}
		case GLFW_KEY_R:
			m_mode = M_R;
			break;
		case GLFW_KEY_S:
			m_mode = M_S;
			break;
		case GLFW_KEY_T:
			m_mode = M_T;
			break;
	}

	return eventHandled;
}

glm::mat4 A2::createProjMatrix(
	float fov,
	float n,
	float f
) {
	mat4 M = mat4(1.0f);
	float w = 2 * tan(fov/2.0f) * n;
	float h = w;
	M[0] = vec4(2*n/w, 0.0f, 0.0f, 0.0f);
	M[1] = vec4(0.0f, 2*n/h, 0.0f, 0.0f);
	M[2] = vec4(0.0f, 0.0f, (n+f)/(n-f), -2*f*n/(f-n));
	M[3] = vec4(0.0f, 0.0f ,-1.0f, 0.0f);
	return glm::transpose(M);
}

// change of basis from world to given basis
glm::mat4 A2::createToLocalMatrix (
	const glm::vec4 u,
	const glm::vec4 v,
	const glm::vec4 w,
	const glm::vec4 e
) {
	mat4 M_1 = mat4(1.0f);
	mat4 M_2 = mat4(1.0f);
	M_1[0] = u;
	M_1[1] = v;
	M_1[2] = w;
	M_2[0][3] = - e.x;
	M_2[1][3] = - e.y;
	M_2[2][3] = - e.z;
	return glm::transpose(M_1 * M_2);
}

// viewport to window transformation matrix
glm::mat4 A2::createWinMatrix(
	const glm::vec2 tl,
	const glm::vec2 br
) {
	mat4 M = mat4(1.0f);
	vec2 dim = br - tl;
	vec2 center = (tl + br)/2.0f;
	M[0][0] = dim[0]/2.0f;
	M[1][1] = -dim[1]/2.0f;
	M[3][0] = center[0];
	M[3][1] = center[1];
	return M;
}

void A2::reset() {
	m_currentLineColour = vec3(0.0f);
	M_rotation = mat4(1.0f);
	M_translate = mat4(1.0f);
	M_scale = mat4(1.0f);
	m_view_dir = vec3(0.0f, 0.0f, -1.0f);
	m_eye_pos = vec3(0.0f, 0.0f, 15.0f);
	m_near = 0.5f;
	m_far = 10.0f;
	m_fov = PI / 6.0f;
	M = mat4(1.0f);
	octahedronVertices[0] = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	octahedronVertices[1] = vec4(-1.0f, 0.0f, 0.0f, 1.0f);
	octahedronVertices[2] = vec4(0.0f, 1.0f, 0.0f, 1.0f);
	octahedronVertices[3] = vec4(0.0f, -1.0f, 0.0f, 1.0f);
	octahedronVertices[4] = vec4(0.0f, 0.0f, 1.0f, 1.0f);
	octahedronVertices[5] = vec4(0.0f, 0.0f, -1.0f, 1.0f);
	local_x = X_VECTOR;
	local_y = Y_VECTOR;
	local_z = Z_VECTOR;
	local_o = ORIGIN;
	view_x = X_VECTOR;
	view_y = Y_VECTOR;
	view_z = Z_VECTOR;
	eye = EYE;
	V = createToLocalMatrix(view_x, view_y, view_z,eye);
	P = createProjMatrix(m_fov, m_near, m_far);
	vp_tl = vec2(-(1-MARGIN), (1-MARGIN));
	vp_br = vec2((1-MARGIN), -(1-MARGIN));
}

void A2::printMatrix(const glm::mat4 m) {
	std::cout << glm::to_string(m) << std::endl;
}
