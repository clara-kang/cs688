// Fall 2018

#include "A1.hpp"
#include "cs488-framework/GlErrorCheck.hpp"

#include <iostream>

#include <sys/types.h>
#include <unistd.h>
#define STB_IMAGE_IMPLEMENTATION
#include  "stb_image.h"

#include <imgui/imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define INITIAL_CAMERA_POS glm::vec3(0.0f, 2.*float(DIM)*2.0*M_SQRT1_2, float(DIM)*2.0*M_SQRT1_2)

using namespace glm;
using namespace std;

static const size_t DIM = 16;
const float AVATAR_SCALE = 0.5f;
const float SIZE_CHANGE = 0.1f;
const float GRID_ANGLE_CHANGE = 0.006f;
const vec3 DEFAULT_FLOOR_COLOR = vec3(0.0f, 0.0f, 0.5f);
const vec3 DEFAULT_AVATAR_COLOR = vec3(1.0f, 0.0f, 0.0f);
const vec3 DEFAULT_CUBE_COLOR = vec3(0.5f, 0.5f, 0.5f);
const float ZOOM_FACTOR_CHANGE = 0.1f;
const float MIN_ZOOM_FACTOR = 0.5f;
const float MAX_ZOOM_FACTOR = 5.0f;
const float AVATAR_GRID_DISPLACEMENT = 1.0f - AVATAR_SCALE / 2.0f;
const float AMBIENT = 0.5f;
const float ATTENUATION = 0.8f;
const float SHININESS = 4.0f;
const float STRENGTH = 1.0f;

//----------------------------------------------------------------------------------------
// Constructor
A1::A1()
	: current_col( 0 ),
	  m ( Maze(DIM) ),
		M_Cube_Scale( mat4(1.0f) ),
		grid_rotation( mat4(1.0f) ),
		floor_col( DEFAULT_FLOOR_COLOR ),
		avatar_col( DEFAULT_AVATAR_COLOR ),
		cube_col( DEFAULT_CUBE_COLOR ),
		zoom_factor( 1 ),
		persistent_rotation_dir( 0 ),
		maze_created( false ),
		shift_down( false ),
		m_mouseButtonActive( false )
{
	colour[0] = floor_col.x;
	colour[1] = floor_col.y;
	colour[2] = floor_col.z;
	avatar_pos[0] = 0;
	avatar_pos[1] = 0;
	M_Avatar_Scale = glm::scale(M_Avatar_Scale, AVATAR_SCALE * vec3(1.0f));
	light_dir = glm::normalize(vec3(1.0f, 1.0f, 2.0f));
	eye_dir = glm::normalize(INITIAL_CAMERA_POS);
}

//----------------------------------------------------------------------------------------
// Destructor
A1::~A1()
{}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A1::init()
{
	// Initialize random number generator
	int rseed=getpid();
	srandom(rseed);
	// Print random number seed in case we want to rerun with
	// same random numbers
	cout << "Random number seed = " << rseed << endl;

	// Set the background colour.
	glClearColor( 0.3, 0.5, 0.7, 1.0 );

	// Build the shader
	m_shader.generateProgramObject();
	m_shader.attachVertexShader(
		getAssetFilePath( "VertexShader.vs" ).c_str() );
	m_shader.attachFragmentShader(
		getAssetFilePath( "FragmentShader.fs" ).c_str() );
	m_shader.link();

	// Set up the uniforms
	P_uni = m_shader.getUniformLocation( "P" );
	V_uni = m_shader.getUniformLocation( "V" );
	M_uni = m_shader.getUniformLocation( "M" );
	col_uni = m_shader.getUniformLocation( "colour" );
	light_uni = m_shader.getUniformLocation( "lightDir");
	ambient_uni = m_shader.getUniformLocation( "ambient");
	atten_uni = m_shader.getUniformLocation( "attenuation");
	eye_uni = m_shader.getUniformLocation( "EyePosition");
	shininess_uni = m_shader.getUniformLocation( "shininess");
	strength_uni = m_shader.getUniformLocation( "strength");

	initGrid();

	// Set up initial view and projection matrices (need to do this here,
	// since it depends on the GLFW window being set up correctly).
	view = glm::lookAt(
		INITIAL_CAMERA_POS,
		glm::vec3( 0.0f, 0.0f, 0.0f ),
		glm::vec3( 0.0f, 1.0f, 0.0f ) );

	proj = glm::perspective(
		glm::radians( 30.0f ),
		float( m_framebufferWidth ) / float( m_framebufferHeight ),
		1.0f, 1000.0f );
}

void A1::initGrid()
{
	size_t sz = 3 * 2 * 2 * (DIM+3);

	float *verts = new float[ sz ];
	size_t ct = 0;
	for( int idx = 0; idx < DIM+3; ++idx ) {
		verts[ ct ] = -1;
		verts[ ct+1 ] = 0;
		verts[ ct+2 ] = idx-1;
		verts[ ct+3 ] = DIM+1;
		verts[ ct+4 ] = 0;
		verts[ ct+5 ] = idx-1;
		ct += 6;

		verts[ ct ] = idx-1;
		verts[ ct+1 ] = 0;
		verts[ ct+2 ] = -1;
		verts[ ct+3 ] = idx-1;
		verts[ ct+4 ] = 0;
		verts[ ct+5 ] = DIM+1;
		ct += 6;
	}

	float cube_verts_pos[] = {
		0, 0, -1, 0, 1, -1, 0, 1, 0,
		0, 0, -1, 0, 1, 0, 0, 0, 0,
		0, 1, 0, 0, 1, -1, -1, 1, -1,
		0, 1, 0, -1, 1, -1, -1, 1, 0,
		-1, 0, 0, -1, 1, 0, -1, 1, -1,
		-1, 0, 0, -1, 1, -1, -1, 0, -1,
		0, 0, 0, 0, 1, 0, -1, 1, 0,
		0, 0, 0, -1, 1, 0, -1, 0, 0,
		-1, 0, -1, -1, 1, -1, 0, 1, -1,
		-1, 0, -1, 0, 1, -1, 0, 0, -1
	};

	float cube_verts_normals[] = {
		1, 0, 0, 1, 0, 0, 1, 0, 0,
		1, 0, 0, 1, 0, 0, 1, 0, 0,
		0, 1, 0, 0, 1, 0, 0, 1, 0,
		0, 1, 0, 0, 1, 0, 0, 1, 0,
		-1, 0, 0, -1, 0, 0, -1, 0, 0,
		-1, 0, 0, -1, 0, 0, -1, 0, 0,
		0, 0, 1, 0, 0, 1, 0, 0, 1,
		0, 0, 1, 0, 0, 1, 0, 0, 1,
		0, 0, -1, 0, 0, -1, 0, 0, -1,
		0, 0, -1, 0, 0, -1, 0, 0, -1
	};

	float cube_verts_tex_coords[] {
		0.5, 0, 0.5, 0.5, 0, 0.5,
		0.5, 0, 0, 0.5, 0, 0,
		0.5, 0, 0.5, 0.5, 0, 0.5,
		0.5, 0, 0, 0.5, 0, 0,
		0.5, 0, 0.5, 0.5, 0, 0.5,
		0.5, 0, 0, 0.5, 0, 0,
		0.5, 0, 0.5, 0.5, 0, 0.5,
		0.5, 0, 0, 0.5, 0, 0,
		0.5, 0, 0.5, 0.5, 0, 0.5,
		0.5, 0, 0, 0.5, 0, 0,
	};

	float floor_verts[] = {-1, 0, -1, -1, 0, DIM+1, DIM+1, 0, -1, DIM+1, 0, DIM+1};

	float floor_verts_normals[] = {0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0};

	float floor_verts_tex_coords[] = {
		0.0, 0.0, 0.0, 0.5, 0.5, 0.0, 0.5, 0.5
	};

	// Create the vertex array to record buffer assignments.
	glGenVertexArrays( 1, &m_grid_vao );
	glBindVertexArray( m_grid_vao );

	// Create the cube vertex buffer
	glGenBuffers( 1, &m_grid_vbo );
	glBindBuffer( GL_ARRAY_BUFFER, m_grid_vbo );
	glBufferData( GL_ARRAY_BUFFER, sz*sizeof(float),
		verts, GL_STATIC_DRAW );

	// Specify the means of extracting the position values properly.
	GLint posAttrib = m_shader.getAttribLocation( "VertexPosition" );
	GLint normalAttrib = m_shader.getAttribLocation( "VertexNormal" );
	GLint texAttrib = m_shader.getAttribLocation( "in_tex_coord" );
	glEnableVertexAttribArray( posAttrib );
	glVertexAttribPointer( posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr );

	// Create the vertex array for cube geometry
	glGenVertexArrays(1, &m_cube_vao);
	glBindVertexArray( m_cube_vao);

	// Create the vertex buffer for the cubes
	glGenBuffers(1, &m_cube_vbo);
	glBindBuffer( GL_ARRAY_BUFFER, m_cube_vbo);
	glBufferData( GL_ARRAY_BUFFER, sizeof(cube_verts_pos)+sizeof(cube_verts_normals)
		+sizeof(cube_verts_tex_coords), NULL, GL_STATIC_DRAW);
	glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(cube_verts_pos), cube_verts_pos);
	glBufferSubData( GL_ARRAY_BUFFER, sizeof(cube_verts_pos),
		sizeof(cube_verts_normals), cube_verts_normals);
	glBufferSubData( GL_ARRAY_BUFFER, sizeof(cube_verts_pos)+sizeof(cube_verts_normals),
		sizeof(cube_verts_tex_coords), cube_verts_tex_coords);

	// Specify the means of extracting the position values properly.
	glEnableVertexAttribArray( posAttrib );
	glVertexAttribPointer( posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr );
	glEnableVertexAttribArray( normalAttrib );
	glVertexAttribPointer( normalAttrib, 3, GL_FLOAT, GL_TRUE, 0,
		(void *)sizeof(cube_verts_pos) );
	glEnableVertexAttribArray( texAttrib );
	glVertexAttribPointer( texAttrib, 2, GL_FLOAT, GL_FALSE, 0,
			(void *)(sizeof(cube_verts_pos)+sizeof(cube_verts_normals)));

	// Create vertex array for floor geometry
	glGenVertexArrays( 1, &m_floor_vao );
	glBindVertexArray( m_floor_vao );

	// Create the floor vertex buffer
	glGenBuffers( 1, &m_floor_vbo );
	glBindBuffer( GL_ARRAY_BUFFER, m_floor_vbo );
	glBufferData( GL_ARRAY_BUFFER, sizeof(floor_verts) + sizeof(floor_verts_normals)
		+ sizeof(floor_verts_tex_coords), NULL, GL_STATIC_DRAW );
	glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(floor_verts), floor_verts);
	glBufferSubData( GL_ARRAY_BUFFER, sizeof(floor_verts),
			sizeof(floor_verts_normals), floor_verts_normals);
	glBufferSubData( GL_ARRAY_BUFFER, sizeof(floor_verts) + sizeof(floor_verts_normals),
		sizeof(floor_verts_tex_coords), floor_verts_tex_coords);

	// Specify the means of extracting the position values properly.
	glEnableVertexAttribArray( posAttrib );
	glVertexAttribPointer( posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr );
	glEnableVertexAttribArray( normalAttrib );
	glVertexAttribPointer( normalAttrib, 3, GL_FLOAT, GL_TRUE, 0,
		(void *)sizeof(floor_verts) );
	glEnableVertexAttribArray( texAttrib );
	glVertexAttribPointer( texAttrib, 2, GL_FLOAT, GL_FALSE, 0,
		(void *)(sizeof(floor_verts)+sizeof(floor_verts_normals)));

	// Reset state to prevent rogue code from messing with *my*
	// stuff!
	glBindVertexArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

	m_texture_floor = LoadTexture("Assets/grass_texture.jpg", m_texture_floor);
	m_texture_cube = LoadTexture("Assets/rock_texture.jpeg", m_texture_cube);
	// OpenGL has the buffer now, there's no need for us to keep a copy.
	delete [] verts;

	CHECK_GL_ERRORS;
}

void A1::newMaze() {
		m.digMaze();
		if (!maze_created) {
			maze_created = true;
		}
		// reset wall height to default
		M_Cube_Scale[1][1] = 1.0f;
		// Move avatar to start position
		avatar_pos[1] = getStartPosY();
		avatar_pos[0] = 0;
}
//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void A1::appLogic()
{
	// Place per frame, application logic here ...
	if (persistent_rotation_dir != 0) {
		grid_rotation = glm::rotate ( grid_rotation,
			GRID_ANGLE_CHANGE * persistent_rotation_dir, vec3(0.0, 1.0, 0.0));
	}
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A1::guiLogic()
{
	// We already know there's only going to be one window, so for
	// simplicity we'll store button states in static local variables.
	// If there was ever a possibility of having multiple instances of
	// A1 running simultaneously, this would break; you'd want to make
	// this into instance fields of A1.
	static bool showTestWindow(false);
	static bool showDebugWindow(true);

	ImGuiWindowFlags windowFlags(ImGuiWindowFlags_AlwaysAutoResize);
	float opacity(0.5f);

	ImGui::Begin("Debug Window", &showDebugWindow, ImVec2(100,100), opacity, windowFlags);
		if( ImGui::Button( "Quit Application" ) ) {
			glfwSetWindowShouldClose(m_window, GL_TRUE);
		}
		if( ImGui::Button( "Reset" ) ) {
			reset();
		}

		// Colour widgets
		if( ImGui::ColorEdit3( "##Colour", colour ) ) {
			switch (current_col) {
				case 0: floor_col = vec3(colour[0], colour[1], colour[2]);
					break;
				case 1: avatar_col = vec3(colour[0], colour[1], colour[2]);
					break;
				case 2: cube_col = vec3(colour[0], colour[1], colour[2]);
					break;
			}
		}
		ImGui::PushID( 0 );
		if (ImGui::RadioButton( "Floor Color", &current_col, 0 )) {
			colour[0] = floor_col.x;
			colour[1] = floor_col.y;
			colour[2] = floor_col.z;
		}
		ImGui::PushID( 1 );
		if (ImGui::RadioButton( "Avatar Color", &current_col, 1 )) {
			colour[0] = avatar_col.x;
			colour[1] = avatar_col.y;
			colour[2] = avatar_col.z;
		}
		ImGui::PushID( 2 );
		if( ImGui::RadioButton( "Maze Color", &current_col, 2 )) {
			colour[0] = cube_col.x;
			colour[1] = cube_col.y;
			colour[2] = cube_col.z;
		}
		ImGui::PopID();
		ImGui::PopID();
		ImGui::PopID();

/*
		// For convenience, you can uncomment this to show ImGui's massive
		// demonstration window right in your application.  Very handy for
		// browsing around to get the widget you want.  Then look in
		// shared/imgui/imgui_demo.cpp to see how it's done.
		if( ImGui::Button( "Test Window" ) ) {
			showTestWindow = !showTestWindow;
		}
*/

		ImGui::Text( "Framerate: %.1f FPS", ImGui::GetIO().Framerate );

	ImGui::End();

	if( showTestWindow ) {
		ImGui::ShowTestWindow( &showTestWindow );
	}
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A1::draw()
{
	// Create a global transformation for the model (centre it).
	mat4 W;
	W = glm::translate( W, vec3( -float(DIM)/2.0f, 0, -float(DIM)/2.0f ) );

	m_shader.enable();
		glEnable( GL_DEPTH_TEST );

		glUniformMatrix4fv( P_uni, 1, GL_FALSE, value_ptr( proj ) );
		glUniformMatrix4fv( V_uni, 1, GL_FALSE, value_ptr( view ) );
		glUniformMatrix4fv( M_uni, 1, GL_FALSE, value_ptr( grid_rotation * W ) );

		// Just draw the grid for now.
		glBindVertexArray( m_grid_vao );
		// Set uniforms
		glUniform3f( col_uni, 1, 1, 1 );
		glUniform3fv( light_uni, 1, value_ptr(light_dir));
		glUniform1f( ambient_uni, AMBIENT);
		glUniform1f( atten_uni, ATTENUATION);
		glUniform3fv( eye_uni, 1, value_ptr(eye_dir));
		glUniform1f( shininess_uni, SHININESS);
		glUniform1f( strength_uni, STRENGTH);
		glDrawArrays( GL_LINES, 0, (3+DIM)*4 );

		// Draw the floor
		glBindTexture(GL_TEXTURE_2D, m_texture_floor);
		glBindVertexArray( m_floor_vao );
		glUniform3fv( col_uni, 1, value_ptr(floor_col));
		glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

		// Draw the cubes
		glBindTexture(GL_TEXTURE_2D, m_texture_cube);
		glBindVertexArray ( m_cube_vao );
		glUniform3fv( col_uni, 1, value_ptr(cube_col));
		mat4 M_Cube_Translate, M_cube;

		// Iterate over the colums of matrix
		for ( int idx = 0; idx < DIM; idx++ ) {
			// Iterate over the rows of matrix
			for ( int idz = 0; idz < DIM; idz++) {
				if ( m.getValue(idx,idz) ) {
					M_Cube_Translate = glm::translate( W, vec3( float(idx)+1, 0, float(idz)+1) );
					M_cube = grid_rotation * M_Cube_Translate * M_Cube_Scale;
					glUniformMatrix4fv( M_uni, 1, GL_FALSE, value_ptr( M_cube ) );
					glDrawArrays( GL_TRIANGLES, 0, 10*3)	;
				}
			}
		}

		// Draw the avatar
		glm::mat4 M_Avatar_Translate, M_Avatar;
		M_Avatar_Translate = glm::translate( W, vec3(avatar_pos[0] +
			AVATAR_GRID_DISPLACEMENT, 0, avatar_pos[1] + AVATAR_GRID_DISPLACEMENT));
		M_Avatar = grid_rotation * M_Avatar_Translate * M_Avatar_Scale;
		glUniform3fv( col_uni, 1, value_ptr(avatar_col));
		glUniformMatrix4fv( M_uni, 1, GL_FALSE, value_ptr( M_Avatar));
		glDrawArrays ( GL_TRIANGLES, 0, 10*3);

		// Highlight the active square.
	m_shader.disable();

	// Restore defaults
	glBindVertexArray( 0 );

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A1::cleanup()
{}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A1::cursorEnterWindowEvent (
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
bool A1::mouseMoveEvent(double xPos, double yPos)
{
	bool eventHandled(false);
	if (!ImGui::IsMouseHoveringAnyWindow()) {
		// rotate the grid with mouse drag
		if (m_mouseButtonActive) {
			grid_rotation = glm::rotate ( grid_rotation,
				(float)(ImGui::GetIO().MousePosPrev.x - xPos) * GRID_ANGLE_CHANGE, vec3(0.0, 1.0, 0.0));
		}
	}

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool A1::mouseButtonInputEvent(int button, int actions, int mods) {
	bool eventHandled(false);

	if( actions == GLFW_PRESS && button == 0) {
		if (!ImGui::IsMouseHoveringAnyWindow()) {
			// The user clicked in the window.  If it's the left
			// mouse button, initiate a rotation.
			m_mouseButtonActive = true;
			persistent_rotation_dir = 0.0f;
		}
	}

	// Mouse release, check is mouse is moving to set persistent rotation
	if ( actions == GLFW_RELEASE && button == 0) {
		m_mouseButtonActive = false;
		if (ImGui::GetIO().MouseDelta.x < 0) {
			persistent_rotation_dir = -1.0f;
		} else if (ImGui::GetIO().MouseDelta.x > 0 ) {
			persistent_rotation_dir = 1.0f;
		} else {
		}

	}

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool A1::mouseScrollEvent(double xOffSet, double yOffSet) {
	bool eventHandled(false);

	// Zoom in or out.
	zoom_factor = std::max( MIN_ZOOM_FACTOR, zoom_factor + (float)yOffSet*ZOOM_FACTOR_CHANGE );
	zoom_factor = std::min( MAX_ZOOM_FACTOR, zoom_factor );
	updateViewMatrix (zoom_factor);

	return eventHandled;
}

void A1::updateViewMatrix (float zoomFactor) {
	view = glm::lookAt(
		INITIAL_CAMERA_POS * zoom_factor,
		glm::vec3( 0.0f, 0.0f, 0.0f ),
		glm::vec3( 0.0f, 1.0f, 0.0f ) );
}
//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles window resize events.
 */
bool A1::windowResizeEvent(int width, int height) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool A1::keyInputEvent(int key, int action, int mods) {
	bool eventHandled(false);

	// Fill in with event handling code...
	if( action == GLFW_PRESS ) {
		// Respond to some key events.
		// D key creates a new Maze
		if (key == GLFW_KEY_D) {
			newMaze();
		}
		// Space key inscrease wall height by 1 unit
		if (key == GLFW_KEY_SPACE) {
			M_Cube_Scale[1][1] += SIZE_CHANGE;
			eventHandled = true;
		}
		// Backspace key decrease wall height by 1 unit
		if (key == GLFW_KEY_BACKSPACE) {
			if ( M_Cube_Scale[1][1] >= SIZE_CHANGE) {
				M_Cube_Scale[1][1] -= SIZE_CHANGE;
			}
			eventHandled = true;
		}
		// Q key closes the application
		if (key == GLFW_KEY_Q) {
			glfwSetWindowShouldClose(m_window, GL_TRUE);
			eventHandled = true;
		}

		// Set shift down is left or right shift is pressed
		if (key == GLFW_KEY_LEFT_SHIFT || key == GLFW_KEY_RIGHT_SHIFT) {
			shift_down = true;
		}

		// Down and Up Key to move avatar down and up, only move when maze created
		if ((key == GLFW_KEY_DOWN || key == GLFW_KEY_UP) && maze_created) {
			// Set next potential position
			int next_pos_z ;
			if (key == GLFW_KEY_DOWN) {
				next_pos_z = std::min(avatar_pos[1]+1, (int)DIM-1);
			} else {
				next_pos_z = std::max(avatar_pos[1]-1, 0);
			}
			// If avatar is not in the maze, or if next position in the maze is
			// not a wall move to next position
			if ( m.getValue(avatar_pos[0], next_pos_z) == 0) {
				avatar_pos[1] = next_pos_z;
			// If next position is a wall, and shift is down, remove wall,
			// move to next position
			} else if (m.getValue(avatar_pos[0], next_pos_z) == 1 && shift_down) {
				m.setValue( avatar_pos[0], next_pos_z, 0 );
				avatar_pos[1] = next_pos_z;
			}
			// Left and Right Key to move avatar left and right
		} else if ((key == GLFW_KEY_LEFT || key == GLFW_KEY_RIGHT) && maze_created) {
			// Set next potential position
			int next_pos_x;
			if (key == GLFW_KEY_LEFT) {
				next_pos_x = std::max(avatar_pos[0]-1, 0);
			} else {
				next_pos_x = std::min(avatar_pos[0]+1, (int)DIM-1);
			}
			// If avatar is not in the maze, or if next position in the maze is
			// not a wall move to next position
			if ( m.getValue(next_pos_x, avatar_pos[1]) == 0) {
				avatar_pos[0] = next_pos_x;
			// If next position is a wall, and shift is down, remove wall,
			// move to next position
			} else if (m.getValue(next_pos_x, avatar_pos[1]) == 1 && shift_down) {
				m.setValue( next_pos_x, avatar_pos[1], 0);
				avatar_pos[0] = next_pos_x;
			}
		}

		// R key resets transformation and color
		if (key == GLFW_KEY_R) {
			reset();
			eventHandled = true;
		}
	}

	if (action == GLFW_RELEASE) {
		// Set shift down to false if shift is released
		if (key == GLFW_KEY_LEFT_SHIFT || key == GLFW_KEY_RIGHT_SHIFT) {
			shift_down = false;
		}
	}

	return eventHandled;
}

void A1::reset () {
	// 	Reset grid rotation
	persistent_rotation_dir = 0.0f;
	grid_rotation = mat4(1.0f);
	// Reset view
	zoom_factor = 1.0f;
	updateViewMatrix (zoom_factor);
	// Reset maze
	m.reset();
	maze_created = false;
	// Reset avatar position
	avatar_pos[0] = 0;
	avatar_pos[1] = 0;
	// Reset colors
	floor_col = DEFAULT_FLOOR_COLOR;
	avatar_col = DEFAULT_AVATAR_COLOR;
	cube_col = DEFAULT_CUBE_COLOR;
	colour[0] = floor_col.x;
	colour[1] = floor_col.y;
	colour[2] = floor_col.z;
	// Reset UI
	current_col = 0;
}

GLuint A1::LoadTexture(const char* filename, GLuint texture) {
	if (!glIsTexture(texture)) {
			glGenTextures(1, &texture);
	}
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int width, height, nrChannels;
	unsigned char *data = stbi_load(filename,
		&width, &height, &nrChannels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
			GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
	} else {
			cout << "Failed to load texture" << endl;
	}
	stbi_image_free(data);
	return texture;
}

int A1::getStartPosY() {
	for (int i = 0; i < DIM; i++) {
		if (m.getValue(0, i) == 0) {
			return i;
		}
	}
	return -1;
}
