#include "Window.h"
#include <iostream>
#include "Shader.h"
#include "Texture.h"
#include "Mesh.h"

void render() 
{
	Window w = Window(1920, 1080);

	float coords[] = {
		//positions			//texture
		1.0f,  1.0f, 0.0f,	1.0f,1.0f, // top right
		1.0f, 0.0f, 0.0f,  1.0f,0.0f, // bottom right
		0.0f, 0.00f, 0.0f, 0.0f,0.0f, // bottom left
		0.0f,  1.0f, 0.0f, 0.0f,1.0f // top left 
	};
	unsigned int indices[] = {  // note that we start from 0!
		0, 1, 3,   // first triangle
		1, 2, 3    // second triangle
	};
	UINT32 vbo, vao, ebo;

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


	glGenBuffers(1, &vbo);
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(coords), coords, GL_STATIC_DRAW);

	Vertex some[2];
	some[0].normal = glm::vec3(1);
	some[0].position = glm::vec3(1);
	some[0].texCoords = glm::vec2(1);
	some[1].normal = glm::vec3(1);
	some[1].position = glm::vec3(1);
	some[1].texCoords = glm::vec2(1);
	//Mesh mesh(some);

	Shader s = Shader("text.vert", "text.frag");
	Texture t = Texture("olle.jpg");
	t.id = 5; //default
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);


	int offset = -100;

	while (!w.windowShouldClose()) {
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

	/*	float timeValue = glfwGetTime();
		float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
		glm::vec4 greenColor = glm::vec4(greenValue, 0.0f, 0.5f, 1.0f);

		offset++;
		// draw our first triangle
		//glBindVertexArray(vao); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
		//glDrawArrays(GL_TRIANGLES, 0, 3);
		// glBindVertexArray(0); // no need to unbind it every time 

		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	*/
		glBindVertexArray(vao);
		s.use();
		t.bind();
		//s.setUniform("ourColor", greenColor);
		//s.setUniform("offset", offset);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		t.unbind();

		w.update();
	}
	

}

int main()
{
	render();

	system("pause");
	return 0;
}