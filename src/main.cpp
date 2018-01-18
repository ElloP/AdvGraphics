#include "Window.h"
#include "vector.h"
#include "Shader.h"

void render()
{
	Window w(800, 600);
	
	
	while(!w.windowShouldClose())
	{
		w.processInput();
		//do stuff
		w.update();
	}
}

int main()
{
	render();
	return 0;
}
