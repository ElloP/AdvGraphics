#include "Window.h"
#include "vector.h"

void render()
{
	Window w = Window(800, 600);
	while(!w.windowShouldClose())
	{
		glClearColor(0.5, 0.1, 0.3, 1);
		glClear(GL_COLOR_BUFFER_BIT);
		w.update();
	}
}

int main()
{
	render();
	return 0;
}
