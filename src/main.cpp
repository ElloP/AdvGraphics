#include "Window.h"
#include "Shader.h"
#include "Material.h"
#include "glm/gtc/matrix_transform.hpp"
#include "Texture.h"
#include "Camera.h"
#include "VertexData.h"


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

// shaders paths
const char* lightvsPath = "C:\\Skola\\Advanced Graphics\\AdvGraphics\\src\\Shaders\\light.vs";
const char* lightfsPath = "C:\\Skola\\Advanced Graphics\\AdvGraphics\\src\\Shaders\\light.fs";
const char* lampvsPath = "C:\\Skola\\Advanced Graphics\\AdvGraphics\\src\\Shaders\\lamp.vs";
const char* lampfsPath = "C:\\Skola\\Advanced Graphics\\AdvGraphics\\src\\Shaders\\lamp.fs";


// texture paths
char* texture1path = "C:\\Skola\\Advanced Graphics\\AdvGraphics\\src\\text.jpg";
char* texture2path = "C:\\Skola\\Advanced Graphics\\AdvGraphics\\src\\images.jpg";



void render()
{
	Window w(WIDTH, HEIGHT);

	glfwSetCursorPosCallback(w.getWindow(), mouse_callback);
	glfwSetScrollCallback(w.getWindow(), scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(w.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);


	Material m;
	m.gold();
	Shader lightShader(lightvsPath, lightfsPath);
	Shader lampShader(lampvsPath, lampfsPath);
	Texture t1(texture1path);
	t1.id = 0;
	Texture t2(texture2path);
	t2.id = 1;

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
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) (6 * sizeof(float)));
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


	glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

	while(!w.windowShouldClose())
	{
		static float rot = glm::radians(44.0f);

		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		

		processInput(w.getWindow());

		glClearColor(0.0, 0.2, 0.3, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
		// set cube
		lightShader.use();
		lightShader.setUniform("objectColor", glm::vec3(1.0f, 0.5f, 0.31f));
		lightShader.setUniform("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		lightShader.setUniform("model", model);
		lightShader.setUniform("view", camera.GetViewMatrix());
		projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
		lightShader.setUniform("projection", projection);
		lightShader.setUniform("lightPosition", lightPos);
		lightShader.setUniform("viewPos", camera.Position);

		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		

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
