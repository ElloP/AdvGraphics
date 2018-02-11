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
#include "FullScreenQuad.h"

//#define STENCIL_ON

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



// I HAVE NO IDEA WHY THESE NEEDS TO BE STRINGS, AND THEN c_str() and not simply const char* lightvsPath = (root + "src/Shaders/light.vs").c_str();
// shaders paths
std::string lightvsPath = (root + "src/Shaders/light.vs");
std::string lightfsPath = (root + "src/Shaders/light.fs");
std::string lampvsPath = (root + "src/Shaders/lamp.vs");
std::string lampfsPath = (root + "src/Shaders/lamp.fs");
std::string outlinefsPath = (root + "src/Shaders/outline.fs");
std::string outlinevsPath = (root + "src/Shaders/outline.vs");
std::string blackfsPath = (root + "src/Shaders/black.fs");
std::string blackvsPath = (root + "src/Shaders/black.vs");



// texture paths
std::string texture1path = (root + "src/container2.png");
std::string texture2path = (root + "src/container2_specular.png");
std::string texturePebble = (root + "src/Textures/Pebbles_004_COLOR.jpg");
std::string texturePebbleNormal = (root + "src/Textures/Pebbles_004_NRM.jpg");

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
	Shader outlineShader(outlinevsPath.c_str(),outlinefsPath.c_str());
	Shader blackShader(blackvsPath.c_str(), blackfsPath.c_str());
	
	Texture t1(texture1path.c_str());
	t1.id = 0;
	Texture t2(texture2path.c_str());
	t2.id = 1;
	Texture t3(texturePebbleNormal.c_str());
	t3.id = 2;

	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,0.0f,0.0f),
		glm::vec3(7.0f,5.0f,0.0f)
	};

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
	//glDisable(GL_CULL_FACE);

#ifdef STENCIL_ON
	glEnable(GL_STENCIL_TEST);
#endif
	
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
#ifdef STENCIL_ON
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF);
#else
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#endif
		
		//RAY MARCHING 
		//blackShader.use();
		//FullscreenQuad::drawFullscreenQuad();


		// set lamp
		lampShader.use();
		glm::mat4 model(1);
		lightPos = glm::mat3(glm::rotate(glm::mat4(1), glm::radians(0.10f), glm::vec3(0.0f, 1.0f, 0.0f))) * lightPos;
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f));
		lampShader.setUniform("model", model);
		lampShader.setUniform("view", camera.GetViewMatrix());
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
		lampShader.setUniform("projection", projection);



		glBindVertexArray(lampVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		

		t1.bind();
		t2.bind();
		t3.bind();
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
		
		//env_map.draw(camera.GetViewMatrix(), projection);
		
	/*	s.setUniform("view", camera.GetViewMatrix());
		
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
		s.setUniform("projection", projection);

		s.setUniform("inColor", glm::vec4(1.0f,0.0,0.0, 1));
		s.setUniform("blend", sin(blendAmount));
		
		//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		
		t1.bind();
		t2.bind();
		glBindVertexArray(cubeVAO);
		for (int i = 0; i < 2; i++) {
			float rot = 30.0f * i;
			glm::mat4 model(1);
			model = glm::translate(model, cubePositions[i]);
			model = glm::rotate(model, rot, glm::vec3(1, 3, 1));
			model = glm::scale(model, glm::vec3(3, 3, 3));
			s.setUniform("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		*/

#ifdef STENCIL_ON
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00);
		glDisable(GL_DEPTH_TEST);
	
		//draw upscaled objects
		outlineShader.use();
		outlineShader.setUniform("view", camera.GetViewMatrix());
		projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
		outlineShader.setUniform("projection", projection);

		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(3.3f, 3.3f, 3.3f));
		outlineShader.setUniform("model", model);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		model = glm::translate(model, glm::vec3(1.5, 1.5, 1.5));
		model = glm::scale(model, glm::vec3(1.1f, 1.1f, 1.1f));
		cubeShader.setUniform("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glStencilMask(0xFF);
		glEnable(GL_DEPTH_TEST);
#endif

		//env_map.draw(camera.GetViewMatrix(), projection);
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
