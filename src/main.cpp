#include "Window.h"
#include "Shader.h"
#include "Material.h"
#include "glm/gtc/matrix_transform.hpp"
#include "Texture.h"
#include "Camera.h"
#include "VertexData.h"
#include "EnvironmentMap.h"
#include "root.h"
#include "Light.h"
#include "ParticleGalaxy.h"

#ifndef M_PI
#	define M_PI 3.14159265358979323846f
#endif 

const int WIDTH = 1920;
const int HEIGHT = 1080;

//callback
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 10.0f));
float lastX = WIDTH / 2.0f;
float lastY = HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// particles


// I HAVE NO IDEA WHY THESE NEEDS TO BE STRINGS, AND THEN c_str() and not simply const char* lightvsPath = (root + "src/Shaders/light.vs").c_str();
// shaders paths
std::string lightvsPath = (root + "src/Shaders/light.vs");
std::string lightfsPath = (root + "src/Shaders/light.fs");
std::string lampvsPath = (root + "src/Shaders/lamp.vs");
std::string lampfsPath = (root + "src/Shaders/lamp.fs");
std::string particlevsPath = (root + "src/Shaders/particle.vs");
std::string particlefsPath = (root + "src/Shaders/particle.fs");
std::string hazevsPath = (root + "src/Shaders/haze.vs");
std::string hazefsPath = (root + "src/Shaders/haze.fs");

// texture paths
std::string texture1path = (root + "src/container2.png");
std::string texture2path = (root + "src/container2_specular.png");

std::string particlePath = (root + "src/explosion.png");

float uniform_randf(const float from, const float to) {
	return from + (to - from) * float(rand()) / float(RAND_MAX);
}

float randf() {
	return float(rand()) / float(RAND_MAX);
}

void render()
{
	Window w(WIDTH, HEIGHT);

	glfwSetCursorPosCallback(w.getWindow(), mouse_callback);
	glfwSetScrollCallback(w.getWindow(), scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(w.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	Material m;
	m.tutorial();
	Shader cubeShader(lightvsPath.c_str(), lightfsPath.c_str());
	Shader lampShader(lampvsPath.c_str(), lampfsPath.c_str());
	Shader particleShader(particlevsPath.c_str(), particlefsPath.c_str());
	Shader hazeShader(hazevsPath.c_str(), hazefsPath.c_str());
	Texture t1(texture1path.c_str());
	t1.id = 0;

	Texture t2(texture2path.c_str());
	t2.id = 1;

	Texture t3(particlePath.c_str());
	t3.id = 0;

	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,0.0f,0.0f),
		glm::vec3(7.0f,5.0f,0.0f)
	};

	// Setting up particle galaxy with particle systems
	ParticleGalaxy particleGalaxy;
	ParticleSystem particleSystem(100, &particleShader, &t3); //testsystem

	particleGalaxy.add(&particleSystem);

	unsigned int VBO, cubeVAO, lampVAO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(cubeVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesCubeColorTexture), verticesCubeColorTexture, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) 0);
	glEnableVertexAttribArray(0);
	// normals
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) (3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) (6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// light
	glGenVertexArrays(1, &lampVAO);
	glBindVertexArray(lampVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	
	float blendAmount = 0;

	
	EnvironmentMap env_map(
		&(root + "src/Cubemap/posx.jpg")[0u],
		&(root + "src/Cubemap/negx.jpg")[0u],
		&(root + "src/Cubemap/posy.jpg")[0u],
		&(root + "src/Cubemap/negy.jpg")[0u],
		&(root + "src/Cubemap/posz.jpg")[0u],
		&(root + "src/Cubemap/negz.jpg")[0u]
	);

	glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

	Light dirLight;
	dirLight.tutorial(lightPos);
	PointLight pointLight;
	pointLight.light = dirLight;
	pointLight.constant = 1.0f;
	pointLight.linear = 0.09f;
	pointLight.quadratic = 0.032f;

	while(!w.windowShouldClose())
	{
		static float rot = glm::radians(44.0f);

		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		

		processInput(w.getWindow());

		glClearColor(0.0, 0.2, 0.3, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
		
		// set lamp
		lampShader.use();
		glm::mat4 model(1);
		lightPos = glm::mat3(glm::rotate(glm::mat4(1), glm::radians(0.10f), glm::vec3(0.0f, 1.0f, 0.0f))) * lightPos;
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f));
		lampShader.setUniform("model", model);
		lampShader.setUniform("view", camera.GetViewMatrix());
		projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
		lampShader.setUniform("projection", projection);

		glBindVertexArray(lampVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		t1.bind();
		t2.bind();
		// set cube
		cubeShader.use();
		cubeShader.setUniform("objectColor", glm::vec3(1.0f, 0.5f, 0.31f));
		cubeShader.setUniform("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		cubeShader.setUniform("model", model);
		cubeShader.setUniform("view", camera.GetViewMatrix());
		projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
		cubeShader.setUniform("projection", projection);
		cubeShader.setUniform("viewPos", camera.Position);
		
		cubeShader.setUniform("material.shininess", m.shininess);

		pointLight.light.position = lightPos;

		cubeShader.setUniform("dirLight.direction", glm::vec3(-0.2f,-1.0f,-0.3f));//dirLight.position);
		cubeShader.setUniform("dirLight.ambient", dirLight.ambient);
		cubeShader.setUniform("dirLight.diffuse", dirLight.diffuse);
		cubeShader.setUniform("dirLight.specular", dirLight.specular);

		cubeShader.setUniform("pointLight.position", pointLight.light.position);
		cubeShader.setUniform("pointLight.ambient", pointLight.light.ambient);
		cubeShader.setUniform("pointLight.diffuse", pointLight.light.diffuse);
		cubeShader.setUniform("pointLight.specular", pointLight.light.specular);
		cubeShader.setUniform("pointLight.constant", pointLight.constant);
		cubeShader.setUniform("pointLight.linear", pointLight.linear);
		cubeShader.setUniform("pointLight.quadratic", pointLight.quadratic);
		

		cubeShader.setUniform("spotLight.position", camera.Position);
		cubeShader.setUniform("spotLight.direction", camera.Front);
		cubeShader.setUniform("spotLight.cutoff", glm::cos(glm::radians(10.0f)));
		cubeShader.setUniform("spotLight.outerCutoff", glm::cos(glm::radians(13.5f)));
		cubeShader.setUniform("spotLight.ambient", glm::vec3(.1f));
		cubeShader.setUniform("spotLight.diffuse", glm::vec3(.8f));
		cubeShader.setUniform("spotLight.specular", glm::vec3(1.0f));

		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		model = glm::translate(model, glm::vec3(1.5, 1.5, 1.5));
		cubeShader.setUniform("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// set environment map
		env_map.draw(camera.GetViewMatrix(), projection);

		// set particles
		for (int i = 0; i < 10; i++) {
			const float theta = uniform_randf(0.f, 2.f * M_PI);
			const float u = uniform_randf(-1.f, 1.f);
			glm::vec3 pos = glm::vec3(sqrt(1.f - u * u) * cosf(theta), u, sqrt(1.f - u * u) * sinf(theta));

			Particle particle;
			particle.velocity = pos * 2.0f;
			particle.pos = pos * 2.0f;
			particle.life_length = 5;
			particle.lifetime = 0;
			particleSystem.spawn(particle);
		}

		particleShader.use();
		particleShader.setUniform("P", projection);
		particleShader.setUniform("screen_x", float(WIDTH));
		particleShader.setUniform("screen_y", float(HEIGHT));

		particleSystem.process_particles(deltaTime);
		particleSystem.draw(camera.GetViewMatrix());

		// update
		w.update();
		blendAmount+=0.01;
		rot+=0.1;
	}
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}
// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

int main()
{
	render();
	return 0;
}

