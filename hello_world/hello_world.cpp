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
OpenGL Hello World
*************************
This example draws a yellow rectangle on a blue background.
If anything goes wrong it will display a red image and print errors to the console
*/
int main(int argc, char* argv[]){
	//Create an OpenGL window and set up a context with proper debug output
	//	This varies based on platform, we use a couple libraries to do it for us
	GLFWwindow* window = init(800,600,"OpenGL Hello World");
	if(window == nullptr){
		//initialization failed
		return -1;
	}
	//set the clear color to a nice blue :)
	glClearColor(0.1f,0.3f,0.6f,1.0f);

	//create a simple shader program which tells the GPU how to draw our data

	//first get the source for the vertex shader
	GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	std::string vertexSource = readContentsOfFile("simple.vert");
	const char* vertexSourcePtr = vertexSource.c_str();
	const GLint vertexSourceLength = vertexSource.size();
	glShaderSource(vertexShaderId,1,&vertexSourcePtr,&vertexSourceLength); //Give OpenGL the source code
	//now we can compile it
	glCompileShader(vertexShaderId);
	if(!checkCompile(vertexShaderId)){ //check status & get any messages the shader compiler might have generated
		printf("compiling vertex shader failed :(");
		waitForExit(window);
		return -1;
	}
	//now the fragment shader
	GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
	std::string fragmentSource = readContentsOfFile("simple.frag");
	const char* fragmentSourcePtr = fragmentSource.c_str();
	const GLint fragmentSourceLength = fragmentSource.size();
	glShaderSource(fragmentShaderId,1,&fragmentSourcePtr,&fragmentSourceLength);
	//compile it
	glCompileShader(fragmentShaderId);
	if(!checkCompile(fragmentShaderId)){ //check status & get any messages the shader compiler might have generated
		printf("compiling fragment shader failed :(");
		waitForExit(window);
		return -1;
	}
	//now we link the shaders together into a program
	GLuint shaderProgramId = glCreateProgram();
	glAttachShader(shaderProgramId,vertexShaderId);
	glAttachShader(shaderProgramId,fragmentShaderId);
	//Tell OpenGL what input it should expect to the shader program, in this case one attribute with the position
	glBindAttribLocation(shaderProgramId,0,"in_Position"); 
	//linking puts everything together so it's ready for use
	glLinkProgram(shaderProgramId);
	if(!checkLink(shaderProgramId)){//check status & get any messages the shader compiler might have generated
		printf("linking shader program failed :(");
		waitForExit(window);
		return -1;
	}

	//Now we need to make a rectangle for the GPU to draw
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
	//here's some points that make a rectangle, arranged like this:
	// (1)-----(2)
	//  |    /  |
	//  |  /    |
	// (3)-----(4)
	// We use a triangle strip so points 2 & 3 are part of both triangles
	float vertices[] = {
		-0.5,-0.5, 0.0,//x,y,z
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

	//All our setup is done, now we can enter our normal draw loop
	auto main_loop = [=]() {
		//first poll for events
		glfwPollEvents();
		//clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		//tell OpenGL to use the program we made earlier for drawing
		glUseProgram(shaderProgramId);
		//use our vertex data declared by the vertex array object for drawing
		glBindVertexArray(vao);
		//Draw!
		glDrawArrays(GL_TRIANGLE_STRIP, //We're using triangle strips
			0,  //start with the first vertex
			4); //draw 4 vertices (all of the ones in our buffer in our case)
		//finally, update the screen with what we've drawn
		glfwSwapBuffers(window);
	};

#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop_arg(&RenderLoopCallback, new std::function<void()>(main_loop), -1, false);
#else
	while(!glfwWindowShouldClose(window)){
		main_loop();
	}
#endif
	return 0;
}