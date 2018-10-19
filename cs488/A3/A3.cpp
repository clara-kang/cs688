// Fall 2018

#include "A3.hpp"
#include "scene_lua.hpp"
using namespace std;

#include "cs488-framework/GlErrorCheck.hpp"
#include "cs488-framework/MathUtils.hpp"
#include "GeometryNode.hpp"
#include "JointNode.hpp"

#include <imgui/imgui.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <map>
using namespace glm;

#define PI 3.14159f

static bool show_gui = true;
static bool quit = false;
static bool undo = false;
static bool redo = false;
static bool reset_pos = false;
static bool reset_rot = false;
static bool reset_joint = false;
static bool reset_all = false;
static bool draw_circle = false;
static bool z_buffer = true;
static bool bculling = false;
static bool fculling = false;
static int rotate_x_axis = 0;

static int stack_cnt = 0;
static int stack_current_index = 0;
const size_t CIRCLE_PTS = 48;
const float ROTATION_SCALE_FACTOR = 1.0f;
const float INPUT_SCALE_FACTOR = 0.001f;

static glm::vec3 *m_color_map;
static bool *m_selected_obj;
//----------------------------------------------------------------------------------------
// Constructor
A3::A3(const std::string & luaSceneFile)
	: m_luaSceneFile(luaSceneFile),
	  m_positionAttribLocation(0),
	  m_normalAttribLocation(0),
	  m_vao_meshData(0),
	  m_vbo_vertexPositions(0),
	  m_vbo_vertexNormals(0),
	  m_vao_arcCircle(0),
	  m_vbo_arcCircle(0),
		m_mode(PO),
		trackball_o(vec3(0.0f)),
		trackball_rotate(false),
		aspect(1024.0f/768.0f),
		m_select_mode(false),
		root_translation(mat4(1.0f)),
		root_rotation(mat4(1.0f)),
		current_joint_rotation(mat4(1.0f))
{

}

//----------------------------------------------------------------------------------------
// Destructor
A3::~A3()
{
	delete [] m_color_map;
}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A3::init()
{
	// Set the background colour.
	glClearColor(0.85, 0.85, 0.85, 1.0);

	createShaderProgram();

	glGenVertexArrays(1, &m_vao_arcCircle);
	glGenVertexArrays(1, &m_vao_meshData);
	enableVertexShaderInputSlots();

	processLuaSceneFile(m_luaSceneFile);

	// Load and decode all .obj files at once here.  You may add additional .obj files to
	// this list in order to support rendering additional mesh types.  All vertex
	// positions, and normals will be extracted and stored within the MeshConsolidator
	// class.
	unique_ptr<MeshConsolidator> meshConsolidator (new MeshConsolidator{
			getAssetFilePath("cube.obj"),
			getAssetFilePath("sphere.obj"),
			getAssetFilePath("suzanne.obj")
	});


	// Acquire the BatchInfoMap from the MeshConsolidator.
	meshConsolidator->getBatchInfoMap(m_batchInfoMap);

	// Take all vertex data within the MeshConsolidator and upload it to VBOs on the GPU.
	uploadVertexDataToVbos(*meshConsolidator);

	mapVboDataToVertexShaderInputLocations();

	initPerspectiveMatrix();

	initViewMatrix();

	initLightSources();

	//extractRootTransMatrices();

	initJointPointers(*m_rootNode);

	// for(JointPointer jp : jointPointers) {
	// 	cout << jp.m_nodeId<<endl;
	// 	cout << endl;
	// }
	initNodeLookupRec(*m_rootNode);
	initJointTransformsRec(*m_rootNode);

	glEnable(GL_CULL_FACE);
	// Exiting the current scope calls delete automatically on meshConsolidator freeing
	// all vertex data resources.  This is fine since we already copied this data to
	// VBOs on the GPU.  We have no use for storing vertex data on the CPU side beyond
	// this point.
	m_color_map = new vec3[(*m_rootNode).totalSceneNodes()];
	createColorMap();
	m_selected_obj = new bool[(*m_rootNode).totalSceneNodes()];
	for (int i = 0; i < (*m_rootNode).totalSceneNodes(); i++) {
		m_selected_obj[i] = false;
	}
	// set root transforms
	root_scale = (*m_rootNode).m_scale;
	root_rotation = (*m_rootNode).m_rotation;
	root_translation = (*m_rootNode).m_translation;
}

//----------------------------------------------------------------------------------------
void A3::processLuaSceneFile(const std::string & filename) {
	// This version of the code treats the Lua file as an Asset,
	// so that you'd launch the program with just the filename
	// of a puppet in the Assets/ directory.
	// std::string assetFilePath = getAssetFilePath(filename.c_str());
	// m_rootNode = std::shared_ptr<SceneNode>(import_lua(assetFilePath));

	// This version of the code treats the main program argument
	// as a straightforward pathname.
	m_rootNode = std::shared_ptr<SceneNode>(import_lua(filename));
	if (!m_rootNode) {
		std::cerr << "Could Not Open " << filename << std::endl;
	}
}

//----------------------------------------------------------------------------------------
void A3::createShaderProgram()
{
	m_shader.generateProgramObject();
	m_shader.attachVertexShader( getAssetFilePath("VertexShader.vs").c_str() );
	m_shader.attachFragmentShader( getAssetFilePath("FragmentShader.fs").c_str() );
	m_shader.link();

	m_shader_select.generateProgramObject();
	m_shader_select.attachVertexShader( getAssetFilePath("simple_VertexShader.vs").c_str() );
	m_shader_select.attachFragmentShader( getAssetFilePath("simple_FragmentShader.fs").c_str() );
	m_shader_select.link();

	m_shader_arcCircle.generateProgramObject();
	m_shader_arcCircle.attachVertexShader( getAssetFilePath("arc_VertexShader.vs").c_str() );
	m_shader_arcCircle.attachFragmentShader( getAssetFilePath("arc_FragmentShader.fs").c_str() );
	m_shader_arcCircle.link();
}

// void A3::extractRootTransMatrices() {
// 	vec3 root_s_extract = vec3(glm::length((*m_rootNode).trans[0]), glm::length((*m_rootNode).trans[1]),
// 		glm::length((*m_rootNode).trans[2]));
// 	root_scale = mat4(1.0f);
// 	for (int i = 0; i < 3; i++) {
// 		root_scale[i][i] = root_s_extract[i];
// 	}
// 	mat3 root_r_extract = mat3((*m_rootNode).trans);
// 	for (int i = 0; i < 3; i++) {
// 		root_r_extract[i] = root_r_extract[i] / root_s_extract[i];
// 	}
// 	root_rotation = mat4(vec4(root_r_extract[0], 0.0f), vec4(root_r_extract[1], 0.0f),
// 		vec4(root_r_extract[2], 0.0f), vec4(0.0f, 0.0f, 0.0f, 1.0f));
// 	vec3 root_t_extract = vec3((*m_rootNode).trans[3]);
// 	root_translation = glm::translate(mat4(1.0f), root_t_extract);
// }

void A3::initJointPointers(const SceneNode & root) {
	if (root.children.size() == 0 ) {
		return;
	}
	if (root.m_nodeType == NodeType::JointNode) {
		// cout << "joint node found" << endl;
		const JointNode * jointNode = static_cast<const JointNode *>(&root);
		for (SceneNode *child : root.children) {
			if (child->m_nodeType == NodeType::GeometryNode) {
				// cout << "add joint pointer" << endl;
				JointPointer jp = {
					child->m_nodeId,
					jointNode
				};
				jointPointers.push_back(jp);
			}
		}
	}
	for (SceneNode *child : root.children) {
		initJointPointers(*child);
	}
}

void A3::initNodeLookupRec(SceneNode & root){
	//SceneNode b = root;
	m_node_lookup.insert(std::make_pair(root.m_nodeId, &root));
	for (SceneNode *child : root.children) {
		initNodeLookupRec(*child);
	}
}
void A3::initJointTransformsRec(const SceneNode & root) {
	if (root.m_nodeType == NodeType::JointNode) {
		std::list<JointTransform> list;
		m_joint_affected[root.m_nodeId] = false;
		m_jointTransforms[root.m_nodeId] = list;
	}
	for (SceneNode *child : root.children) {
		initJointTransformsRec(*child);
	}
}

//----------------------------------------------------------------------------------------
void A3::enableVertexShaderInputSlots()
{
	//-- Enable input slots for m_vao_meshData:
	{
		glBindVertexArray(m_vao_meshData);

		// Enable the vertex shader attribute location for "position" when rendering.
		m_positionAttribLocation = m_shader.getAttribLocation("position");
		glEnableVertexAttribArray(m_positionAttribLocation);

		// Enable the vertex shader attribute location for "normal" when rendering.
		m_normalAttribLocation = m_shader.getAttribLocation("normal");
		glEnableVertexAttribArray(m_normalAttribLocation);

		CHECK_GL_ERRORS;
	}

	{
		glBindVertexArray(m_vao_meshData);

		// Enable the vertex shader attribute location for "position" when rendering.
		m_positionAttribLocation_select = m_shader_select.getAttribLocation("position");
		glEnableVertexAttribArray(m_positionAttribLocation_select);

		CHECK_GL_ERRORS;
	}


	//-- Enable input slots for m_vao_arcCircle:
	{
		glBindVertexArray(m_vao_arcCircle);

		// Enable the vertex shader attribute location for "position" when rendering.
		m_arc_positionAttribLocation = m_shader_arcCircle.getAttribLocation("position");
		glEnableVertexAttribArray(m_arc_positionAttribLocation);

		CHECK_GL_ERRORS;
	}

	// Restore defaults
	glBindVertexArray(0);
}

//----------------------------------------------------------------------------------------
void A3::uploadVertexDataToVbos (
		const MeshConsolidator & meshConsolidator
) {
	// Generate VBO to store all vertex position data
	{
		glGenBuffers(1, &m_vbo_vertexPositions);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexPositions);

		glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexPositionBytes(),
				meshConsolidator.getVertexPositionDataPtr(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}

	// Generate VBO to store all vertex normal data
	{
		glGenBuffers(1, &m_vbo_vertexNormals);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexNormals);

		glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexNormalBytes(),
				meshConsolidator.getVertexNormalDataPtr(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}

	// Generate VBO to store the trackball circle.
	{
		glGenBuffers( 1, &m_vbo_arcCircle );
		glBindBuffer( GL_ARRAY_BUFFER, m_vbo_arcCircle );

		float *pts = new float[ 2 * CIRCLE_PTS ];
		for( size_t idx = 0; idx < CIRCLE_PTS; ++idx ) {
			float ang = 2.0 * M_PI * float(idx) / CIRCLE_PTS;
			pts[2*idx] = cos( ang );
			pts[2*idx+1] = sin( ang );
		}

		glBufferData(GL_ARRAY_BUFFER, 2*CIRCLE_PTS*sizeof(float), pts, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}
}

//----------------------------------------------------------------------------------------
void A3::mapVboDataToVertexShaderInputLocations()
{
	// Bind VAO in order to record the data mapping.
	glBindVertexArray(m_vao_meshData);

	// Tell GL how to map data from the vertex buffer "m_vbo_vertexPositions" into the
	// "position" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexPositions);
	glVertexAttribPointer(m_positionAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glVertexAttribPointer(m_positionAttribLocation_select, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	// Tell GL how to map data from the vertex buffer "m_vbo_vertexNormals" into the
	// "normal" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexNormals);
	glVertexAttribPointer(m_normalAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;

	// Bind VAO in order to record the data mapping.
	glBindVertexArray(m_vao_arcCircle);

	// Tell GL how to map data from the vertex buffer "m_vbo_arcCircle" into the
	// "position" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_arcCircle);
	glVertexAttribPointer(m_arc_positionAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void A3::initPerspectiveMatrix()
{
	m_perpsective = glm::perspective(degreesToRadians(60.0f), aspect, 0.1f, 100.0f);
}


//----------------------------------------------------------------------------------------
void A3::initViewMatrix() {
	m_view = glm::lookAt(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f),
			vec3(0.0f, 1.0f, 0.0f));
}

//----------------------------------------------------------------------------------------
void A3::initLightSources() {
	// World-space position
	m_light.position = vec3(-2.0f, 5.0f, 0.5f);
	m_light.rgbIntensity = vec3(0.8f); // White light
}

//----------------------------------------------------------------------------------------
void A3::uploadCommonSceneUniforms() {
	if (!m_select_mode) {
		m_shader.enable();
		{
			//-- Set Perpsective matrix uniform for the scene:
			GLint location = m_shader.getUniformLocation("Perspective");
			glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(m_perpsective));
			CHECK_GL_ERRORS;


			//-- Set LightSource uniform for the scene:
			{
				location = m_shader.getUniformLocation("light.position");
				glUniform3fv(location, 1, value_ptr(m_light.position));
				location = m_shader.getUniformLocation("light.rgbIntensity");
				glUniform3fv(location, 1, value_ptr(m_light.rgbIntensity));
				CHECK_GL_ERRORS;
			}

			//-- Set background light ambient intensity
			{
				location = m_shader.getUniformLocation("ambientIntensity");
				vec3 ambientIntensity(0.05f);
				glUniform3fv(location, 1, value_ptr(ambientIntensity));
				CHECK_GL_ERRORS;
			}
		}
		m_shader.disable();
	} else {
		m_shader_select.enable();
		//-- Set Perpsective matrix uniform for the scene:
		GLint location = m_shader_select.getUniformLocation("Perspective");
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(m_perpsective));
		CHECK_GL_ERRORS;
		m_shader_select.disable();
	}
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void A3::appLogic()
{
	// Place per frame, application logic here ...

	uploadCommonSceneUniforms();

	if( quit ) {
		glfwSetWindowShouldClose(m_window, GL_TRUE);
	}
	if ( undo ) {
		stack_current_index = std::max(stack_current_index - 1, 0);
		// cout << "stack_current_index: " << stack_current_index << endl;
		undo = false;
	}
	if (redo) {
		stack_current_index = std::min(stack_current_index + 1, stack_cnt);
		// cout << "stack_current_index: " << stack_current_index << endl;
		redo = false;
	}
	if ( reset_pos ) {
		cout << "reset_pos " <<endl;
		(*m_rootNode).m_translation = root_translation;
		reset_pos = false;
	}
	if ( reset_rot ) {
		(*m_rootNode).m_rotation = root_rotation;
		reset_rot = false;
	}
	if ( reset_joint ) {
		for (std::map<unsigned int, std::list<JointTransform>>::iterator it=m_jointTransforms.begin();
			it!=m_jointTransforms.end(); ++it) {
			 (it->second).clear();
		}
		reset_joint = false;
	}
	if ( reset_all ) {
		reset_pos = true;
		reset_rot = true;
		reset_joint = true;
		reset_all = false;
	}
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A3::guiLogic()
{
	if( !show_gui ) {
		return;
	}

	static bool firstRun(true);
	if (firstRun) {
		ImGui::SetNextWindowPos(ImVec2(50, 50));
		firstRun = false;
	}
	//ImGui::ShowTestWindow();
	showUI();
}

void A3::showUI() {
	 static bool showDebugWindow(true);
	 ImGuiWindowFlags windowFlags(ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_MenuBar);
	 float opacity(0.5f);

	 ImGui::Begin("Properties", &showDebugWindow, ImVec2(100,100), opacity,
			 windowFlags);
			 if (ImGui::RadioButton( "Position/Orientation (P)", &m_mode, PO )) {
				 m_select_mode = false;
			 }
			 if (ImGui::RadioButton( "Joint (J)", &m_mode, J )) {
				 m_select_mode = true;
			 }
			 ImGui::Indent();
			 ImGui::RadioButton( "x axis", &rotate_x_axis, 1); ImGui::SameLine();
			 ImGui::RadioButton( "y axis", &rotate_x_axis, 0);
			 // menu
			 if (ImGui::BeginMenuBar()) {
				 if (ImGui::BeginMenu("Application")) {
					 static bool show = true;
					 ImGui::MenuItem("Reset Position", "I", &reset_pos);
					 ImGui::MenuItem("Reset Orientation", "O", &reset_rot);
					 ImGui::MenuItem("Reset Joints", "S", &reset_joint);
					 ImGui::MenuItem("Reset All", "A", &reset_all);
					 ImGui::MenuItem("Quit", "Q", &quit);
					 ImGui::EndMenu();
				 }
				 if (ImGui::BeginMenu("Edit")) {
					 static bool show = true;
					 ImGui::MenuItem("Undo", "U", &undo);
					 ImGui::MenuItem("Redo", "R", &redo);

					 ImGui::EndMenu();
				 }
				 if (ImGui::BeginMenu("Options")) {
					 static bool show = true;
					 ImGui::Checkbox("Circle (C)", &draw_circle);
					 ImGui::Checkbox("Z-buffer (Z)", &z_buffer);
					 ImGui::Checkbox("Backface culling (B)", &bculling);
					 ImGui::Checkbox("Frontface culling (F)", &fculling);

					 ImGui::EndMenu();
				 }
				 ImGui::EndMenuBar();
			 }
		 // }
	 ImGui::Text( "Framerate: %.1f FPS", ImGui::GetIO().Framerate );
	 ImGui::End();
 }
//----------------------------------------------------------------------------------------
// Update mesh specific shader uniforms:
static void updateShaderUniforms(
		const ShaderProgram & shader,
		const GeometryNode & node,
		const glm::mat4 & viewMatrix,
		const glm::mat4 & modelMatrix,
		bool select_mode
) {

	shader.enable();
	{
		//-- Set ModelView matrix:
		GLint location = shader.getUniformLocation("ModelView");
		mat4 modelView = viewMatrix * modelMatrix;
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(modelView));
		CHECK_GL_ERRORS;
		if (!select_mode) {
			//-- Set NormMatrix:
			location = shader.getUniformLocation("NormalMatrix");
			mat3 normalMatrix = glm::transpose(glm::inverse(mat3(modelView)));
			glUniformMatrix3fv(location, 1, GL_FALSE, value_ptr(normalMatrix));
			CHECK_GL_ERRORS;


			//-- Set Material values:
			location = shader.getUniformLocation("material.kd");
			vec3 kd = node.material.kd;
			glUniform3fv(location, 1, value_ptr(kd));
			CHECK_GL_ERRORS;
			location = shader.getUniformLocation("material.ks");
			vec3 ks = node.material.ks;
			glUniform3fv(location, 1, value_ptr(ks));
			CHECK_GL_ERRORS;
			location = shader.getUniformLocation("material.shininess");
			glUniform1f(location, node.material.shininess);
			CHECK_GL_ERRORS;
		} else {
			vec3 color = m_color_map[node.m_nodeId];
			location = shader.getUniformLocation("obj_color");
			if (!m_selected_obj[node.m_nodeId]) {
				color = 0.5f*color;
			}
			glUniform3fv(location, 1, value_ptr(color));
			CHECK_GL_ERRORS;
		}
	}
	shader.disable();

}

int A3::getIndexInColorMap (vec3 color) {
	for (int i = 0; i < (*m_rootNode).totalSceneNodes(); i++) {
		vec3 res1 = m_color_map[i] - color;
		vec3 res2 = 0.5f * m_color_map[i] - color;
		if (length(res1) < 0.01 || length(res2) < 0.01) {
			return i;
		}
	}
	return -1;
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A3::draw() {
	if (z_buffer) {
		glEnable( GL_DEPTH_TEST );
	}
	glEnable( GL_CULL_FACE );
	if ( fculling && !bculling ) {
		glCullFace(GL_FRONT);
	} else if ( bculling && !fculling ) {
		glCullFace(GL_BACK);
	} else if ( bculling && fculling ) {
		glCullFace ( GL_FRONT_AND_BACK );
	}
	renderSceneGraph(*m_rootNode);


	glDisable( GL_DEPTH_TEST );
	if ( draw_circle ) {
		renderArcCircle();
	}
}

void A3::renderSceneGraphRec(const SceneNode & root, const glm::mat4 & modelMatrix,
	const glm::mat4 & scaleMatrix) {

	if (root.m_nodeType == NodeType::GeometryNode) {
		const GeometryNode * geometryNode = static_cast<const GeometryNode *>(&root);
		if (!m_select_mode) {
			updateShaderUniforms(m_shader, *geometryNode, m_view,
				modelMatrix*root.m_translation*root.m_rotation*root.m_scale*scaleMatrix, m_select_mode);
		} else {
			// cout << "modelMatrix of " << root.m_name << ": "<< glm::to_string(glm::transpose(modelMatrix)) << endl;
			// cout << "trans of " << root.m_name << ": "<< glm::to_string(glm::transpose(root.trans)) << endl;
			updateShaderUniforms(m_shader_select, *geometryNode, m_view,
				modelMatrix*root.m_translation*root.m_rotation*root.m_scale*scaleMatrix, m_select_mode);
		}
		// Get the BatchInfo corresponding to the GeometryNode's unique MeshId.
		BatchInfo batchInfo = m_batchInfoMap[geometryNode->meshId];
		//-- Now render the mesh:
		if (!m_select_mode) {
			m_shader.enable();
			glDrawArrays(GL_TRIANGLES, batchInfo.startIndex, batchInfo.numIndices);
			m_shader.disable();
		} else {
			m_shader_select.enable();
			glDrawArrays(GL_TRIANGLES, batchInfo.startIndex, batchInfo.numIndices);
			m_shader_select.disable();
		}

	}
	if (root.children.size() > 0 ) {
		glm::mat4 node_trans = root.m_translation*root.m_rotation;
		if (root.m_nodeType == NodeType::JointNode) {
			mat4 M_final_rotation = mat4(1.0f);
			std::list<JointTransform>::reverse_iterator it;
			int cnt = 0;
			for (it=	m_jointTransforms[root.m_nodeId].rbegin();
				it!=m_jointTransforms[root.m_nodeId].rend(); it++) {
				// only apply transform matrices up to stack_current_index
				if (cnt >= stack_current_index) {
					break;
				}
				M_final_rotation = (*it).trans_matrix * M_final_rotation;
				// M_final_rotation = M_final_rotation * (*it);
				//cout << "trans_matrix: " << glm::to_string((*it).trans_matrix);
				cnt ++;
			}
			node_trans = root.m_translation * M_final_rotation * root.m_rotation;
		}
		for (const SceneNode * node : root.children) {
			const SceneNode * nextRoot = static_cast<const SceneNode *>(node);
			renderSceneGraphRec(*nextRoot, modelMatrix * node_trans, scaleMatrix * root.m_scale);
		}
	}
}

//----------------------------------------------------------------------------------------
void A3::renderSceneGraph(const SceneNode & root) {

	// Bind the VAO once here, and reuse for all GeometryNode rendering below.
	glBindVertexArray(m_vao_meshData);
	//const SceneNode * nextRoot = static_cast<const SceneNode *>(root);
	renderSceneGraphRec(root, mat4(1.0f), mat4(1.0f));

	glBindVertexArray(0);
	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
// Draw the trackball circle.
void A3::renderArcCircle() {
	glBindVertexArray(m_vao_arcCircle);

	m_shader_arcCircle.enable();
		GLint m_location = m_shader_arcCircle.getUniformLocation( "M" );
		glm::mat4 M;
		if( aspect > 1.0 ) {
			M = glm::scale( glm::mat4(), glm::vec3( 0.5/aspect, 0.5, 1.0 ) );
		} else {
			M = glm::scale( glm::mat4(), glm::vec3( 0.5, 0.5*aspect, 1.0 ) );
		}
		glUniformMatrix4fv( m_location, 1, GL_FALSE, value_ptr( M ) );
		glDrawArrays( GL_LINE_LOOP, 0, CIRCLE_PTS );
	m_shader_arcCircle.disable();

	glBindVertexArray(0);
	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A3::cleanup()
{

}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A3::cursorEnterWindowEvent (
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
bool A3::mouseMoveEvent (
		double xPos,
		double yPos
) {
	bool eventHandled(false);
	float x,y;
	if (m_button == GLFW_MOUSE_BUTTON_LEFT && mouse_down && m_mode == PO) {
		float dx = ImGui::GetIO().MouseDelta.x * INPUT_SCALE_FACTOR;
		float dy = -ImGui::GetIO().MouseDelta.y * INPUT_SCALE_FACTOR;
		mat4 M = glm::translate((*m_rootNode).m_translation, vec3(dx, dy, 0.0f));
		(*m_rootNode).m_translation = M;
	}
	if (m_button == GLFW_MOUSE_BUTTON_RIGHT && mouse_down && m_mode == PO) {
		if (aspect >= 1.0f) {
			x = (xPos - m_framebufferWidth/2.0f) * 4.0f * aspect/m_framebufferWidth;
			y = (-yPos + m_framebufferHeight/2.0f) * 4.0f/m_framebufferHeight;
		} else {
			x = ((xPos - m_framebufferWidth/2.0f) * 4.0f)/m_framebufferWidth;
			y = (-yPos + m_framebufferHeight/2.0f) * 4.0f / aspect/m_framebufferHeight;
		}
		if ( x*x + y*y <= 1.0f ) {
			if ( !trackball_rotate ){
				trackball_rotate = true;
				trackball_u = glm::normalize(vec3(x, y, sqrt(1-x*x-y*y)));
			} else {
				vec3 trackball_w = glm::normalize(vec3(x, y, sqrt(1-x*x-y*y)));
				vec3 rotation_axis = glm::normalize(glm::cross(trackball_u, trackball_w));
				if (!isnan(rotation_axis[0]) && !isnan(rotation_axis[1]) && !isnan(rotation_axis[2])) {
					mat4 M = glm::rotate(mat4(1.0f),
						ROTATION_SCALE_FACTOR * acos(glm::dot(trackball_w, trackball_u)),
						rotation_axis);
					(*m_rootNode).m_rotation = M * (*m_rootNode).m_rotation;
				}
				trackball_u = trackball_w;
			}
		}
	}
	if (mouse_down && m_mode == J && m_button == GLFW_MOUSE_BUTTON_RIGHT) {
		float d_rotation;
		vec3 rotation_axis;
		if (rotate_x_axis == 1) {
			rotation_axis = vec3(1.0f, 0.0f, 0.0f);
			d_rotation = ImGui::GetIO().MouseDelta.x * ROTATION_SCALE_FACTOR;
		} else {
			rotation_axis = vec3(0.0f, 1.0f, 0.0f);
			d_rotation = -ImGui::GetIO().MouseDelta.y * ROTATION_SCALE_FACTOR;
		}
		for (std::map<unsigned int, bool>::iterator it=m_joint_affected.begin(); it!=m_joint_affected.end(); ++it) {
			if (it->second) { // If the joint is affected
				JointNode * jn = (JointNode *)m_node_lookup[it->first]; // get the joint
				// get the next angle
				double next_angle;
				if (rotate_x_axis == 0) {
					// cout << "rotation of : " << jn->m_nodeId << ": " << jn->m_joint_y.init << ", max: "
					// 	<<  jn->m_joint_y.max << ", min: " << jn-> m_joint_y.min << endl;
					double current_y = m_jointTransforms[it->first].front().current_y;
					mat4 trans = m_jointTransforms[it->first].front().trans_matrix;
					next_angle = std::min(jn->m_joint_y.max, current_y + d_rotation);
					next_angle = std::max(jn->m_joint_y.min, next_angle);
					// cout << "next_angle: " << next_angle << endl;
					// change top transformation matrix in joint's stack by change in angle
					if (next_angle != current_y) {
						m_jointTransforms[it->first].front().trans_matrix = glm::rotate(trans, (float)(PI*((next_angle - current_y))/180.0f), rotation_axis);
						(m_jointTransforms[it->first]).front().current_y = next_angle;
					}
				} else {
					// cout << "rotation of : " << jn->m_nodeId << ": " << jn->m_joint_x.init << ", max: "
					// 	<<  jn->m_joint_x.max << ", min: " << jn-> m_joint_x.min << endl;
					double current_x = m_jointTransforms[it->first].front().current_x;
					mat4 trans = m_jointTransforms[it->first].front().trans_matrix;
					next_angle = std::min(jn->m_joint_x.max, current_x + d_rotation);
					next_angle = std::max(jn->m_joint_x.min, next_angle);
					// cout << "next_angle: " << next_angle << endl;
					if (next_angle != current_x) {
						m_jointTransforms[it->first].front().trans_matrix = glm::rotate(trans, (float)(PI*((next_angle - current_x))/180.0f), vec3(1.0f, 0.0f, 0.0f));
						(m_jointTransforms[it->first]).front().current_x = next_angle;
					}
				}
			}
		}
	}

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool A3::mouseButtonInputEvent (
		int button,
		int actions,
		int mods
) {
	bool eventHandled(false);
	m_button = button;
	// Fill in with event handling code...
	if ( actions == GLFW_PRESS ) {
		if (!ImGui::IsMouseHoveringAnyWindow()) {
			mouse_down = true;
			if (m_mode == J && m_button == GLFW_MOUSE_BUTTON_LEFT ) {
				uint8_t *pixel_color = new uint8_t[3];
				glReadPixels(ImGui::GetIO().MousePos.x, m_framebufferHeight - ImGui::GetIO().MousePos.y,
					1,1,GL_RGB,GL_UNSIGNED_BYTE,(GLvoid*)pixel_color);
				vec3 color = (1.0f/255.0f)*vec3((float)pixel_color[0], (float)pixel_color[1], (float)pixel_color[2]);
				int obj_index = getIndexInColorMap(color);
				if (obj_index > -1) {
					selectNodes(obj_index);
				}

				delete [] pixel_color;
			}
			if (m_mode == J && m_button == GLFW_MOUSE_BUTTON_RIGHT ) {
				// only push new transformation matrix when there is object selected
				bool push_matrix = false;
				for (int i = 0; i < (*m_rootNode).totalSceneNodes(); i++) {
					if (m_selected_obj[i] == true) {
						push_matrix = true;
						break;
					}
				}
				if (push_matrix) {
					int num_matrices_to_pop = stack_cnt - stack_current_index;
					stack_cnt -= num_matrices_to_pop;
					JointTransform jt;
					// cout << "num_matrices_to_pop: " << num_matrices_to_pop << endl;
					for (std::map<unsigned int, bool>::iterator it=m_joint_affected.begin(); it!=m_joint_affected.end(); ++it) {
						// before pushing new transform, pop all matrices above current index
						for (int i = 0; i < num_matrices_to_pop; i++) {
							m_jointTransforms[it->first].pop_front();
						}
						if (stack_cnt > 0) {
							JointTransform last_trans = m_jointTransforms[it->first].front();
							jt = {
								mat4(1.0f),
								last_trans.current_x,
								last_trans.current_y
							};
						} else {
							// get joint
							JointNode * jn = (JointNode *)m_node_lookup[it->first];
							jt = {
								mat4(1.0f),
								jn->m_joint_x.init,
								jn->m_joint_y.init
							};
						}
						// now push new transform
						m_jointTransforms[it->first].push_front(jt);
					}
					stack_cnt ++;
					stack_current_index ++;
					// cout << "stack_cnt: " << stack_cnt << endl;
					// cout << "stack_current_index: " << stack_current_index << endl;
				}
			}
		}
	}

	if ( actions == GLFW_RELEASE ) {
		if (!ImGui::IsMouseHoveringAnyWindow()) {
			trackball_rotate = false;
			mouse_down = false;
		}
	}

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool A3::mouseScrollEvent (
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
bool A3::windowResizeEvent (
		int width,
		int height
) {
	aspect = ((float)width) / height;
	bool eventHandled(false);
	initPerspectiveMatrix();
	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool A3::keyInputEvent (
		int key,
		int action,
		int mods
) {
	bool eventHandled(false);

	if( action == GLFW_PRESS ) {
		if( key == GLFW_KEY_M ) {
			show_gui = !show_gui;
			eventHandled = true;
		}
		if (key == GLFW_KEY_Q) {
			glfwSetWindowShouldClose(m_window, GL_TRUE);
		} else if (key == GLFW_KEY_U) {
			undo = true;
		} else if (key == GLFW_KEY_R) {
			redo = true;
		} else if (key == GLFW_KEY_I) {
			reset_pos = true;
		} else if (key == GLFW_KEY_O) {
			reset_rot = true;
		} else if (key == GLFW_KEY_S) {
			reset_joint = true;
		} else if (key == GLFW_KEY_A) {
			reset_all = true;
		}

	}
	// Fill in with event handling code...

	return eventHandled;
}

void A3::selectNodes (int node_index) {
	for (JointPointer jp : jointPointers) {
		if (node_index == jp.m_nodeId) {
			//SceneNode b = *jp.joint;
			//m_selected_joints.at(&b) = !m_selected_joints.at(&b); // select joint
			m_joint_affected[jp.joint->m_nodeId] = !m_joint_affected[jp.joint->m_nodeId];
			m_selected_obj[node_index] = !m_selected_obj[node_index];
			// for (SceneNode *child : jp.joint->children) {
			// 	selectChildrenRec(*child); //select all children
			// }
		}
	}
}

// not used
void A3::selectChildrenRec (const SceneNode & root) {
	m_selected_obj[root.m_nodeId] = !m_selected_obj[root.m_nodeId];
	if (root.m_nodeType == NodeType::JointNode) {
		//SceneNode b = root;
		//m_selected_joints.at(&b) = !m_selected_joints.at(&b);
		m_joint_affected[root.m_nodeId] = !m_joint_affected[root.m_nodeId];
	}
	if (root.children.size() > 0) {
		for (SceneNode *child : root.children) {
			selectChildrenRec(*child);
		}
	}
}

void A3::createColorMap () {
	for (int i = 0; i < (*m_rootNode).totalSceneNodes(); i++) {
		m_color_map[i] = (1.0f/255.0f)*vec3(rand() % 254 + 1, rand() % 254 + 1, rand() % 254 + 1);
	}
}
