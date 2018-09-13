// Fall 2018

#include "A1.hpp"
#include "cs488-framework/GlErrorCheck.hpp"

#include <iostream>

#include <sys/types.h>
#include <unistd.h>

#include <imgui/imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtx/pre_xPos_vector.hpp>
#include <glm/gtc/type_ptr.hpp>
//TODO: DELETE
#include <glm/gtx/string_cast.hpp>
#define INITIAL_CAMERA_POS glm::vec3(0.0f, 2.*float(DIM)*2.0*M_SQRT1_2, float(DIM)*2.0*M_SQRT1_2)
using namespace glm;
using namespace std;

static const size_t DIM = 16;
const float AVATAR_SCALE = 0.5f;
const float SIZE_CHANGE = 0.1f;
const float GRID_ANGLE_CHANGE = 0.0125f;
const float ZOOM_FACTOR_CHANGE = 0.1f;
const float MIN_ZOOM_FACTOR = 0.5f;
const float MAX_ZOOM_FACTOR = 5.0f;
const float AVATAR_GRID_DISPLACEMENT = 1.0f - AVATAR_SCALE / 2.0f;

//----------------------------------------------------------------------------------------
// Constructor
A1::A1()
	: current_col( 0 ),
	  m ( Maze(DIM) ),
		M_Cube_Scale( mat4(1.0f) ),
		grid_rotation( mat4(1.0f) ),
		zoom_factor( 1 ),
		persistent_rotation_dir( 0 ),
		maze_created( false ),
		shift_down( false )
{
	colour[0] = 0.0f;
	colour[1] = 0.0f;
	colour[2] = 0.0f;
	avatar_pos[0] = -1;
	avatar_pos[1] = -1;
	M_Avatar_Scale = glm::scale(M_Avatar_Scale, AVATAR_SCALE * vec3(1.0f));
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


	// DELETE FROM HERE...
	// Maze m(DIM);
	// m.digMaze();
	// m.printMaze();
	// ...TO HERE

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

	float cube_verts[] = {
		0, 0, -1, 0, 1, -1, 0, 1, 0,
		0, 0, -1, 0, 1, 0, 0, 0, 0,
		0, 1, -1, -1, 1, -1, 0, 1, 0,
		-1, 1, -1, -1, 1, 0, 0, 1, 0,
		-1, 1, 0, -1, 1, -1, -1, 0, -1,
		-1, 1, 0, -1, 0, -1, -1, 0, 0,
		0, 1, 0, -1, 1, 0, 0, 0, 0,
		-1, 1, 0, -1, 0, 0, 0, 0, 0,
		-1, 1, -1, 0, 1, -1, 0, 0, -1,
		-1, 1, -1, 0, 0, -1, -1, 0, -1
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
	GLint posAttrib = m_shader.getAttribLocation( "position" );
	glEnableVertexAttribArray( posAttrib );
	glVertexAttribPointer( posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr );

	// Create the vertex array for cube_verts
	glGenVertexArrays(1, &m_cube_vao);
	glBindVertexArray( m_cube_vao);

	// Create the vertex buffer for the cubes
	glGenBuffers(1, &m_cube_vbo);
	glBindBuffer( GL_ARRAY_BUFFER, m_cube_vbo);
	glBufferData( GL_ARRAY_BUFFER, sizeof(cube_verts), cube_verts, GL_STATIC_DRAW);

	// Specify the means of extracting the position values properly.
	glEnableVertexAttribArray( posAttrib );
	glVertexAttribPointer( posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr );

	// Reset state to prevent rogue code from messing with *my*
	// stuff!
	glBindVertexArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

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

		// Eventually you'll create multiple colour widgets with
		// radio buttons.  If you use PushID/PopID to give them all
		// unique IDs, then ImGui will be able to keep them separate.
		// This is unnecessary with a single colour selector and
		// radio button, but I'm leaving it in as an example.

		// Prefixing a widget name with "##" keeps it from being
		// displayed.

		ImGui::PushID( 0 );
		ImGui::ColorEdit3( "##Colour", colour );
		ImGui::SameLine();
		if( ImGui::RadioButton( "##Col", &current_col, 0 ) ) {
			// Select this colour.
		}
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
		glUniform3f( col_uni, 1, 1, 1 );
		glDrawArrays( GL_LINES, 0, (3+DIM)*4 );

		// Draw the cubes
		glBindVertexArray ( m_cube_vao );
		glUniform3f( col_uni, 0.5f, 0.5f, 0.5f);
		mat4 M_Cube_Translate, M_cube;

		// Iterate over the colums of matrix
		for ( int idx = 0; idx < DIM; idx++ ) {
			// Iterate over the rows of matrix
			for ( int idz = 0; idz < DIM; idz++) {
				if ( m.getValue(idx,idz) ) {
					M_Cube_Translate = glm::translate( W, vec3( float(idx)+1, 0, float(idz)+1) );
					M_cube = grid_rotation * M_Cube_Translate * M_Cube_Scale;
					glUniformMatrix4fv( M_uni, 1, GL_FALSE, value_ptr( M_cube ) );
					glDrawArrays( GL_TRIANGLES, 0, 10*3*3)	;
				}
			}
		}

		// Draw the avatar
		glm::mat4 M_Avatar_Translate, M_Avatar;
		M_Avatar_Translate = glm::translate( W, vec3(avatar_pos[0] +
			AVATAR_GRID_DISPLACEMENT, 0, avatar_pos[1] + AVATAR_GRID_DISPLACEMENT));
		M_Avatar = grid_rotation * M_Avatar_Translate * M_Avatar_Scale;
		glUniform3f( col_uni, 1, 0, 0);
		glUniformMatrix4fv( M_uni, 1, GL_FALSE, value_ptr( M_Avatar));
		glDrawArrays ( GL_TRIANGLES, 0, 10*3*3);

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
		// Put some code here to handle rotations.  Probably need to
		// check whether we're *dragging*, not just moving the mouse.
		// Probably need some instance variables to track the current
		// rotation amount, and maybe the previous X position (so
		// that you can rotate relative to the *change* in X.
		if (m_mouseButtonActive) {
			grid_rotation = glm::rotate ( grid_rotation,
				(float)(xPos - ImGui::GetIO().MousePosPrev.x) * GRID_ANGLE_CHANGE, vec3(0.0, 1.0, 0.0));
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
	if ( actions == GLFW_RELEASE) {
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
	view = glm::lookAt(
		INITIAL_CAMERA_POS * zoom_factor,
		glm::vec3( 0.0f, 0.0f, 0.0f ),
		glm::vec3( 0.0f, 1.0f, 0.0f ) );

	return eventHandled;
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
				next_pos_z = std::min(avatar_pos[1]+1, (int)DIM);
			} else {
				next_pos_z = std::max(avatar_pos[1]-1, -1);
			}
			// If avatar is not in the maze, or if next position in the maze is
			// not a wall move to next position
			if ( outOfMaze(avatar_pos[0], next_pos_z) ||
				m.getValue(avatar_pos[0], next_pos_z) == 0) {
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
				next_pos_x = std::max(avatar_pos[0]-1, -1);
			} else {
				next_pos_x = std::min(avatar_pos[0]+1, (int)DIM);
			}
			// If avatar is not in the maze, or if next position in the maze is
			// not a wall move to next position
			if ( outOfMaze(next_pos_x, avatar_pos[1]) ||
				m.getValue(next_pos_x, avatar_pos[1]) == 0) {
				avatar_pos[0] = next_pos_x;
			// If next position is a wall, and shift is down, remove wall,
			// move to next position
			} else if (m.getValue(next_pos_x, avatar_pos[1]) == 1 && shift_down) {
				m.setValue( next_pos_x, avatar_pos[1], 0);
				avatar_pos[0] = next_pos_x;
			}
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

bool A1::outOfMaze (int x, int y) {
	return (x < 0 || y < 0 || x > (int)DIM - 1 || y > (int)DIM - 1);
}
