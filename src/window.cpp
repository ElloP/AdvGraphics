#include "window.h"
#include <iostream>

Window::Window(int _width, int _height) : width(_width), height(_height)
{
	//Initiate OpenGL context with version 4.1
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(width, height, "AdvGraphics", nullptr, nullptr);

	if(window == nullptr)
	{
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
	}

	glfwMakeContextCurrent(window);

	if(!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
	{
		std::cerr << "Failed to intialize GLAD" << std::endl;
	}
}

Window::~Window()
{
	glfwTerminate();
}

//Note(Olle): for now, only shuts down when user presses escape
/*void Window::processInput()
{
	if(glfwGetKey(this->window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(this->window, true);
}

bool Window::processKey(char c) {
	switch (c) {
	case 'A': return glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS;
	case 'W': return glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;
	case 'S': return glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS;
	case 'D': return glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;
	default: return false;

	}
}
*/
void Window::update(void)
{

	glfwSwapBuffers(window);
	glfwPollEvents();
}
