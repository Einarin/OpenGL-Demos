/***************************************************************************
Copyright (c) 2015 John Dickinson

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
***************************************************************************/

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <cstdio>
#include <string>
#include "infrastructure.h"
#include "shader.h"
#ifdef __EMSCRIPTEN__
#include <emscripten.h>

void RenderLoopCallback(void* arg) {
	(*static_cast<std::function<void()>*>(arg))();
}

#endif

/*
OpenGL Model View Projection
*************************
This demo shows off simple usage of the model, view, and projection matrices.

Things to try:
- render a different shape
- create an animated shape by making the geometry or matrices change
*/

int width = 800;
int height = 600;
glm::mat4 projectionMatrix;
void onResize(GLFWwindow* window, int w, int h){
	width = w;
	height = h;
	//calculate an updated perspective projection matrix
	projectionMatrix = glm::perspective(75.f,float(w)/float(h),0.01f,10.f);
	//tell OpenGL to make our viewport match the window size
	glViewport(0,0,w,h);
	glClear(GL_COLOR_BUFFER_BIT);
	glfwSwapBuffers(window);
}
int main(int argc, char* argv[]){
	//Create an OpenGL window and set up a context with proper debug output
	//	This varies based on platform, we use a couple libraries to do it for us
	GLFWwindow* window = init(800,600,"OpenGL Model View Projection");
	if(window == nullptr){
		//initialization failed
		return -1;
	}
	//add a callback so we know when the window is resized
	onResize(window,width,height); //and call it once to set initial values
	glfwSetFramebufferSizeCallback(window,onResize);

	//set the clear color to a nice blue :)
	glClearColor(0.1f,0.3f,0.6f,1.0f);

	//compile our shader
	std::shared_ptr<Shader> shader = Shader::Create("mvp.vert","depth.frag");
	if(!shader){
		//compiling one of the shaders failed
		waitForExit(window);
		return -1;
	}
	shader->bindAttrib(0,"in_Position");
	if(!shader->link()){
		printf("linking shader program failed :(");
		waitForExit(window);
		return -1;
	}
	//now to pass in our transform matrices we need to get the location of the uniforms
	shader->bind();
	GLint projectionMatrixIndex = glGetUniformLocation(shader->getId(),"in_projectionMatrix");
	GLint modelViewMatrixIndex = glGetUniformLocation(shader->getId(),"in_modelViewMatrix");

	GLuint vao; 
	GLuint vbo; 
	//create a vertex array object
	//	A vertex array object holds OpenGL state related to vertex data
	glGenVertexArrays(1,&vao);
	//create a buffer object to hold some GPU data
	//	we will use it to hold geometry data
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao); //make the vertex array object active so we can modify it
	//bind the buffer object
	//	GL_ARRAY_BUFFER holds per-vertex data, in this case a 3D position
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	//here's some data for us to upload
	float vertices[] = {
		-0.5,-0.5, 0.0,
		 0.5,-0.5, 0.0,
		-0.5, 0.5, 0.0,
		 0.5, 0.5, 0.0
	};
	//This downloads the data to the GPU into the buffer we bound to GL_ARRAY_BUFFER
	//	GL_STATIC_DRAW is a hint to the GPU about what we're going to use the data for
	//	STATIC means that we're going to keep it around for a while and use it multiple times
	//	DRAW means it's data for drawing
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices),vertices,GL_STATIC_DRAW);
	//This tells the GPU how the data is structured.
	//	This is done for each attribute to be passed to the GPU
	glEnableVertexAttribArray(0); //We're specifying parameter 0, in_Position in the vertex shader
	glVertexAttribPointer(0, //We're specifying parameter 0, in_Position in the vertex shader
		3, //Three components to the member since it's a 3D position, the vec3 type in GLSL
		GL_FLOAT, //Each component is a float
		GL_FALSE, //OpenGL can automatically normalize your data if you want
		3 * sizeof(float), //This is the offset in the array between the beginning of each attribute
			//	This makes it possible to pack multiple attributes into one buffer.
			//	You can also bind multiple buffers each with different data
		0); //  This is the offset of the start of data in the buffer
	//Now that we're done we unbind our Vertex Array Object
	glBindVertexArray(0);
	
	//We already set up a perspective projection matrix, now let's make the others
	//set the camera position to (0,1,-3) looking at (0,0,0)
	glm::mat4 viewMatrix = glm::lookAt(glm::vec3(0.f,1.f,-3.f),glm::vec3(0.f,0.f,0.f),glm::vec3(0.f,1.f,0.f));
	glm::mat4 modelMatrix = glm::scale(glm::vec3(2.f));//scale object to double it's initial size

	//Main rendering loop
	auto main_loop = [=]() mutable {
		//first poll for events
		glfwPollEvents();

		//let's make our rectangle spin
		modelMatrix = glm::rotate(modelMatrix,0.5f,glm::vec3(0.f,1.f,0.f));

		//clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		shader->bind();
		glUniformMatrix4fv(projectionMatrixIndex,1,false,glm::value_ptr(projectionMatrix));
		glUniformMatrix4fv(modelViewMatrixIndex,1,false,glm::value_ptr(viewMatrix * modelMatrix));
		glBindVertexArray(vao); //and our vertex data declared by the vertex array object
		//Draw!
		glDrawArrays(GL_TRIANGLE_STRIP, //We're using triangle strips
			0,4); //the first 4 vertices (all of the ones in our buffer in our case)

		//finally, update the screen
		glfwSwapBuffers(window);
	};

#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop_arg(&RenderLoopCallback, new std::function<void()>(main_loop), -1, false);
#else
	while (!glfwWindowShouldClose(window)) {
		main_loop();
	}
#endif
	return 0;
}