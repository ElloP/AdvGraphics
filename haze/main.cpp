

#ifdef _WIN32
extern "C" _declspec(dllexport) unsigned int NvOptimusEnablement = 0x00000001;
#endif

#include <GL/glew.h>
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <chrono>
#include <iostream>

#include <labhelper.h>
#include <imgui.h>
#include <imgui_impl_sdl_gl3.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
using namespace glm;

#include <Model.h>
#include "hdr.h"
#include "fbo.h"
#include "particlesystem.h"




using std::min;
using std::max;

///////////////////////////////////////////////////////////////////////////////
// Various globals
///////////////////////////////////////////////////////////////////////////////
SDL_Window* g_window = nullptr;
float currentTime  = 0.0f;
float previousTime = 0.0f;
float deltaTime    = 0.0f;
bool showUI = false;
int windowWidth, windowHeight;


///////////////////////////////////////////////////////////////////////////////
// Shader programs
///////////////////////////////////////////////////////////////////////////////
GLuint shaderProgram; // Shader for rendering the final image
GLuint simpleShaderProgram; // Shader used to draw the shadow map
GLuint backgroundProgram;
GLuint postProcessProgram;
GLuint particlesProgram;
GLuint onlyParticlesProgram;
GLuint heightfieldProgram;
GLuint cloudProgram;
GLuint filterProgram;
GLuint blurProgram;

///////////////////////////////////////////////////////////////////////////////
// Environment
///////////////////////////////////////////////////////////////////////////////
float environment_multiplier = 1.5f;
GLuint environmentMap, irradianceMap, reflectionMap;
const std::string envmap_base_name = "001";

///////////////////////////////////////////////////////////////////////////////
// Light source
///////////////////////////////////////////////////////////////////////////////
vec3 lightPosition;
vec3 point_light_color = vec3(1.f, 1.f, 1.f);

float point_light_intensity_multiplier = 10000.0f;

///////////////////////////////////////////////////////////////////////////////
// FBO
///////////////////////////////////////////////////////////////////////////////
std::vector<FboInfo> fboList;

///////////////////////////////////////////////////////////////////////////////
// Particle systems
///////////////////////////////////////////////////////////////////////////////

ParticleSystem particleSystem;

///////////////////////////////////////////////////////////////////////////////
// Camera parameters.
///////////////////////////////////////////////////////////////////////////////
vec3 cameraPosition(-70.0f, 50.0f, 70.0f);
vec3 cameraDirection = normalize(vec3(0.0f) - cameraPosition);
float cameraSpeed = 1.0f;

vec3 worldUp(0.0f, 1.0f, 0.0f);

///////////////////////////////////////////////////////////////////////////////
// Models
///////////////////////////////////////////////////////////////////////////////
labhelper::Model *fighterModel = nullptr;
labhelper::Model *landingpadModel = nullptr;
labhelper::Model *sphereModel = nullptr;

mat4 roomModelMatrix;
mat4 landingPadModelMatrix; 
mat4 fighterModelMatrix;

///////////////////////////////////////////////////////////////////////////////
// GUI
///////////////////////////////////////////////////////////////////////////////

bool guiHaze;
bool guiHazeTime;
bool blurClouds;
float guiHazeMagnitude;
float cloudCutoff = 0.001f;
float fakeTime = 0.0f;
bool drawTerr;
int distr = 0;

/////////////////////
// CLOUDS
//////////////////////
GLuint clouds;

void loadShaders(bool is_reload)
{
	GLuint shader = labhelper::loadShaderProgram("../haze/shaders/simple.vert", "../haze/shaders/simple.frag", is_reload);
	if (shader != 0) simpleShaderProgram = shader; 
	shader = labhelper::loadShaderProgram("../haze/shaders/background.vert", "../haze/shaders/background.frag", is_reload);
	if (shader != 0) backgroundProgram = shader;
	shader = labhelper::loadShaderProgram("../haze/shaders/shading.vert", "../haze/shaders/shading.frag", is_reload);
	if (shader != 0) shaderProgram = shader;
}

void loadShaders() {
	backgroundProgram = labhelper::loadShaderProgram("../haze/shaders/background.vert", "../haze/shaders/background.frag");
	shaderProgram = labhelper::loadShaderProgram("../haze/shaders/shading.vert", "../haze/shaders/shading.frag");
	simpleShaderProgram = labhelper::loadShaderProgram("../haze/shaders/simple.vert", "../haze/shaders/simple.frag");
	onlyParticlesProgram = labhelper::loadShaderProgram("../haze/shaders/softparticles.vert", "../haze/shaders/softparticles.frag");
	postProcessProgram = labhelper::loadShaderProgram("../haze/shaders/postprocess.vert", "../haze/shaders/postprocess.frag");
	particlesProgram = labhelper::loadShaderProgram("../haze/shaders/particle.vert", "../haze/shaders/particle.frag");
}

void initGL()
{
	
	///////////////////////////////////////////////////////////////////////
	//		Load Shaders
	///////////////////////////////////////////////////////////////////////
	loadShaders();

	///////////////////////////////////////////////////////////////////////
	// Load models and set up model matrices
	///////////////////////////////////////////////////////////////////////
	fighterModel    = labhelper::loadModelFromOBJ("../scenes/NewShip.obj");
	landingpadModel = labhelper::loadModelFromOBJ("../scenes/landingpad.obj");
	sphereModel     = labhelper::loadModelFromOBJ("../scenes/sphere.obj");

	roomModelMatrix = mat4(1.0f);
	fighterModelMatrix = translate(15.0f * worldUp);
	landingPadModelMatrix = mat4(1.0f);

	///////////////////////////////////////////////////////////////////////
	// Load environment map
	///////////////////////////////////////////////////////////////////////
	const int roughnesses = 8;
	std::vector<std::string> filenames;
	for (int i = 0; i < roughnesses; i++)
		filenames.push_back("../scenes/envmaps/" + envmap_base_name + "_dl_" + std::to_string(i) + ".hdr");

	reflectionMap = labhelper::loadHdrMipmapTexture(filenames);
	environmentMap = labhelper::loadHdrTexture("../scenes/envmaps/" + envmap_base_name + ".hdr");
	irradianceMap = labhelper::loadHdrTexture("../scenes/envmaps/" + envmap_base_name + "_irradiance.hdr");


	glEnable(GL_DEPTH_TEST);	// enable Z-buffering 
	glEnable(GL_CULL_FACE);		// enables backface culling
	glEnable(GL_BLEND);

	///////////////////////////////////////////////////////////////////////////
	// Setup Framebuffers
	///////////////////////////////////////////////////////////////////////////
	int w, h;
	SDL_GetWindowSize(g_window, &w, &h);
	const int numFbos = 5;
	for (int i = 0; i < numFbos; i++) {
		FboInfo fbo = FboInfo();
		fbo.resize(w, h);
		fboList.push_back(fbo);
	}

	SDL_GetWindowSize(g_window, &w, &h);

	///////////////////////////////////////////////////////////////////////////
	// Setup Particle Systems
	///////////////////////////////////////////////////////////////////////////
	GLuint explosion = labhelper::loadParticleTexture("../scenes/explosion.png");
	particleSystem = ParticleSystem(1000, &onlyParticlesProgram, explosion);
}

void debugDrawLight(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix, const glm::vec3 &worldSpaceLightPos)
{
	mat4 modelMatrix = glm::translate(worldSpaceLightPos);
	glUseProgram(shaderProgram);
	labhelper::setUniformSlow(shaderProgram, "modelViewProjectionMatrix", projectionMatrix * viewMatrix * modelMatrix);
	labhelper::render(sphereModel);
	labhelper::setUniformSlow(shaderProgram, "modelViewProjectionMatrix", projectionMatrix * viewMatrix);
	labhelper::debugDrawLine(viewMatrix, projectionMatrix, worldSpaceLightPos);
}


void drawBackground(const mat4 &viewMatrix, const mat4 &projectionMatrix)
{
	glUseProgram(backgroundProgram);
	labhelper::setUniformSlow(backgroundProgram, "environment_multiplier", environment_multiplier);
	labhelper::setUniformSlow(backgroundProgram, "inv_PV", inverse(projectionMatrix * viewMatrix));
	labhelper::setUniformSlow(backgroundProgram, "camera_pos", cameraPosition);
	labhelper::drawFullScreenQuad();
}

void drawScene(GLuint currentShaderProgram, const mat4 &viewMatrix, const mat4 &projectionMatrix, const mat4 &lightViewMatrix, const mat4 &lightProjectionMatrix)
{
	glUseProgram(currentShaderProgram);
	// Light source
	vec4 viewSpaceLightPosition = viewMatrix * vec4(lightPosition, 1.0f);
	labhelper::setUniformSlow(currentShaderProgram, "point_light_color", point_light_color);
	labhelper::setUniformSlow(currentShaderProgram, "point_light_intensity_multiplier", point_light_intensity_multiplier);
	labhelper::setUniformSlow(currentShaderProgram, "viewSpaceLightPosition", vec3(viewSpaceLightPosition));
	labhelper::setUniformSlow(currentShaderProgram, "viewSpaceLightDir", normalize(vec3(viewMatrix * vec4(-lightPosition, 0.0f))));


	// Environment
	labhelper::setUniformSlow(currentShaderProgram, "environment_multiplier", environment_multiplier);

	// camera
	labhelper::setUniformSlow(currentShaderProgram, "viewInverse", inverse(viewMatrix));

	// landing pad 
	labhelper::setUniformSlow(currentShaderProgram, "modelViewProjectionMatrix", projectionMatrix * viewMatrix * landingPadModelMatrix);
	labhelper::setUniformSlow(currentShaderProgram, "modelViewMatrix", viewMatrix * landingPadModelMatrix);
	labhelper::setUniformSlow(currentShaderProgram, "normalMatrix", inverse(transpose(viewMatrix * landingPadModelMatrix)));
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	labhelper::render(landingpadModel);

	// Fighter
	labhelper::setUniformSlow(currentShaderProgram, "modelViewProjectionMatrix", projectionMatrix * viewMatrix * fighterModelMatrix);
	labhelper::setUniformSlow(currentShaderProgram, "modelViewMatrix", viewMatrix * fighterModelMatrix);
	labhelper::setUniformSlow(currentShaderProgram, "normalMatrix", inverse(transpose(viewMatrix * fighterModelMatrix)));

	labhelper::render(fighterModel);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

}


void update(void) {
	particleSystem.update(deltaTime);
}

void display(void)
{
	///////////////////////////////////////////////////////////////////////////
	// Check if window size has changed and resize buffers as needed
	///////////////////////////////////////////////////////////////////////////
	{
		int w, h;
		SDL_GetWindowSize(g_window, &w, &h);
		if (w != windowWidth || h != windowHeight) {
			windowWidth = w;
			windowHeight = h;
		}
	}
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	///////////////////////////////////////////////////////////////////////////
	// setup matrices
	///////////////////////////////////////////////////////////////////////////
	mat4 projMatrix = perspective(radians(45.0f), float(windowWidth) / float(windowHeight), 5.0f, 2000.0f);
	mat4 viewMatrix = lookAt(cameraPosition, cameraPosition + cameraDirection, worldUp);

	vec4 lightStartPosition = vec4(40.0f, 40.0f, 0.0f, 1.0f);
	lightPosition = vec3(rotate(currentTime, worldUp) * lightStartPosition);
	mat4 lightViewMatrix = lookAt(lightPosition, vec3(0.0f), worldUp);
	mat4 lightProjMatrix = perspective(radians(45.0f), 1.0f, 25.0f, 100.0f);

	///////////////////////////////////////////////////////////////////////////
	// Bind the environment map(s) to unused texture units
	///////////////////////////////////////////////////////////////////////////
	glUseProgram(shaderProgram);
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, environmentMap);
	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, irradianceMap);
	glActiveTexture(GL_TEXTURE8);
	glBindTexture(GL_TEXTURE_2D, reflectionMap);
	glActiveTexture(GL_TEXTURE0);

	///////////////////////////////////////////////////////////////////////////
	// Draw from camera
	///////////////////////////////////////////////////////////////////////////
	FboInfo &postProcessBuffer = fboList[0];
	glBindFramebuffer(GL_FRAMEBUFFER, postProcessBuffer.framebufferId);
	glViewport(0, 0, windowWidth, windowHeight);
	glClearColor(0.0, 0.0, 0.0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	drawBackground(viewMatrix, projMatrix);
	drawScene(shaderProgram, viewMatrix, projMatrix, lightViewMatrix, lightProjMatrix);
	debugDrawLight(viewMatrix, projMatrix, vec3(lightPosition));

	///////////////////////////////////////////////////////////////////////////
	// Post processing pass(es)
	///////////////////////////////////////////////////////////////////////////

	FboInfo &hazeOutlineBuffer = fboList[1];
	glBindFramebuffer(GL_FRAMEBUFFER, hazeOutlineBuffer.framebufferId);
	glViewport(0, 0, windowWidth, windowHeight);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(onlyParticlesProgram);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, postProcessBuffer.depthBuffer);

	labhelper::setUniformSlow(onlyParticlesProgram, "P", projMatrix);
	labhelper::setUniformSlow(onlyParticlesProgram, "screen_x", float(windowWidth));
	labhelper::setUniformSlow(onlyParticlesProgram, "screen_y", float(windowHeight));
	
	
	particleSystem.draw(viewMatrix);
	

	///////////////////////////////////////////////////////////////////////////
	// Draw back haze Particles
	///////////////////////////////////////////////////////////////////////////

	FboInfo &hazeParticlesBuffer = fboList[2];
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, windowWidth, windowHeight);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(postProcessProgram);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, postProcessBuffer.colorTextureTargets[0]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, hazeOutlineBuffer.colorTextureTargets[0]);


	labhelper::setUniformSlow(postProcessProgram, "time", guiHazeTime ? currentTime : 1);
	labhelper::setUniformSlow(postProcessProgram, "magnitude", guiHazeMagnitude);
	labhelper::setUniformSlow(postProcessProgram, "particles", guiHaze);
	labhelper::drawFullScreenQuad();
	
}

bool handleEvents(void)
{
	// check events (keyboard among other)
	SDL_Event event;
	bool quitEvent = false;
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT || (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_ESCAPE)) {
			quitEvent = true;
		}
		if (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_g) {
			showUI = !showUI;
		}
		if (event.type == SDL_MOUSEMOTION && !ImGui::IsMouseHoveringAnyWindow()) {
			static int prev_xcoord = event.motion.x;
			static int prev_ycoord = event.motion.y;
			int delta_x = event.motion.x - prev_xcoord;
			int delta_y = event.motion.y - prev_ycoord;

			if (event.button.button & SDL_BUTTON(SDL_BUTTON_LEFT)) {
				float rotationSpeed = 0.005f;
				mat4 yaw = rotate(rotationSpeed * -delta_x, worldUp);
				mat4 pitch = rotate(rotationSpeed * -delta_y, normalize(cross(cameraDirection, worldUp)));
				cameraDirection = vec3(pitch * yaw * vec4(cameraDirection, 0.0f));
			}
			prev_xcoord = event.motion.x;
			prev_ycoord = event.motion.y;
		}
	}

	// check keyboard state (which keys are still pressed)
	const uint8_t *state = SDL_GetKeyboardState(nullptr);
	vec3 cameraRight = cross(cameraDirection, worldUp);

	if (state[SDL_SCANCODE_W]) {
		cameraPosition += cameraSpeed* cameraDirection;
	}
	if (state[SDL_SCANCODE_S]) {
		cameraPosition -= cameraSpeed * cameraDirection;
	}
	if (state[SDL_SCANCODE_A]) {
		cameraPosition -= cameraSpeed * cameraRight;
	}
	if (state[SDL_SCANCODE_D]) {
		cameraPosition += cameraSpeed * cameraRight;
	}
	if (state[SDL_SCANCODE_Q]) {
		cameraPosition -= cameraSpeed * worldUp;
	}
	if (state[SDL_SCANCODE_E]) {
		cameraPosition += cameraSpeed * worldUp;
	}
	return quitEvent;
}

void gui()
{
	// Inform imgui of new frame
	ImGui_ImplSdlGL3_NewFrame(g_window);

	// ----------------- Set variables --------------------------
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	// ----------------------------------------------------------
	ImGui::Checkbox("Fire", &guiHaze);
	ImGui::Checkbox("Affected by time", &guiHazeTime);
	ImGui::DragFloat("Haze Magnitude", &guiHazeMagnitude, 1.0f, 0, 1000);
	// ----------------------------------------------------------
	if (ImGui::Button("Reload shaders"))
		loadShaders();
	// Render the GUI.
	ImGui::Render();
}

int main(int argc, char *argv[])
{
	//NoiseGenerator::generateNoiseTexture();
	g_window = labhelper::init_window_SDL("OpenGL Project");

	initGL();

	bool stopRendering = false;
	auto startTime = std::chrono::system_clock::now();

	while (!stopRendering) {
		//update currentTime
		std::chrono::duration<float> timeSinceStart = std::chrono::system_clock::now() - startTime;
		previousTime = currentTime;
		currentTime  = timeSinceStart.count();
		deltaTime    = currentTime - previousTime;

		// update logic
		update();

		// render to window
		display();

		// Render overlay GUI.
		if (showUI) {
			gui();
		}

		// Swap front and back buffer. This frame will now been displayed.
		SDL_GL_SwapWindow(g_window);

		// check events (keyboard among other)
		stopRendering = handleEvents();
	}
	// Free Models
	labhelper::freeModel(fighterModel);
	labhelper::freeModel(landingpadModel);
	labhelper::freeModel(sphereModel);

	// Shut down everything. This includes the window and all other subsystems.
	labhelper::shutDown(g_window);
	return 0;
}
