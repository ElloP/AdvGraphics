#ifndef FULLSCREEN_QUAD_H
#define FULLSCREEN_QUAD_H

#include "glad/glad.h"
#include "Shader.h"

class FullscreenQuad
{
public:
	static void drawFullscreenQuad() 
	{
		static unsigned int vao = 0;
		static float points[] = {
			-1.0f,-1.0f,
			1.0f, 1.0f,
			-1.0f, 1.0f,
			-1.0f,-1.0f,
			1.0f,-1.0f,
			1.0f, 1.0f
		};
		GLboolean depthTestStatus;

		glGetBooleanv(GL_DEPTH_TEST, &depthTestStatus);
		glDisable(GL_DEPTH_TEST);
		if(!vao)
		{
			glGenVertexArrays(1, &vao);
			glBindVertexArray(vao);
			unsigned int vbo;
			glGenBuffers(1, &vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
			
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float) , (void*)0);
			glEnableVertexAttribArray(0);
		} 
		else 
			glBindVertexArray(vao);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		if(depthTestStatus) 
			glEnable(GL_DEPTH_TEST);
	}
};

#endif
