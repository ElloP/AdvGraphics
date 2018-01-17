#ifndef WINDOW_H_
#define WINDOW_H_

#include <glad/glad.h>
#include <glfw/glfw3.h>

class Window
{
public:
	Window(int _width, int _height);
	~Window();

	void update(void);
	void processInput();

	inline void setWidth(unsigned int _width)
	{
		width = _width;
	}

	inline void setHeight(unsigned int _height)
	{
		height = _height;
	}

	inline int getWidth()
	{
		return width;
	}

	inline int getHeight()
	{
		return height;
	}

	inline bool windowShouldClose()
	{
		return glfwWindowShouldClose(window);
	}

private:
	int width;
	int height;

	GLFWwindow* window;
};

#endif // !WINDOW_H