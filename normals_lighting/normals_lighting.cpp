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
#include "geometry.h"
#ifdef __EMSCRIPTEN__
#include <emscripten.h>

void RenderLoopCallback(void* arg) {
	(*static_cast<std::function<void()>*>(arg))();
}

#endif

/*
OpenGL Normals & Lighting
*************************
This demo shows the use of basic lighting.
It uses Lambertian diffuse and Blinn-Phong specular.
A geometry shader is used to draw the normal vectors and the light vector.

Things to try:
	- change the animation of the cube or light or control them with the mouse
	- render a different shape, observe how the lighting effects it
	- add more lights
	- implement a more advanced lighting model, see http://renderwonk.com/publications/s2010-shading-course/
	  for an excellent treatment of physically based shading
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

	//set the clear color to ambient
	glClearColor(0.1f,0.1f,0.1f,1.0f);

	//compile our shader
	std::shared_ptr<Shader> lightingShader = Shader::Create("mvpNormals.vert","lighting.frag");
	if(!lightingShader){
		//compiling one of the shaders failed
		waitForExit(window);
		return -1;
	}
	lightingShader->bindAttrib(0,"in_Position");
	lightingShader->bindAttrib(1,"in_Normal");
	lightingShader->bindAttrib(2,"in_TexCoord");
	if(!lightingShader->link()){
		printf("linking shader program failed :(");
		waitForExit(window);
		return -1;
	}
	//now to pass in our transform matrices we need to get the location of the uniforms
	lightingShader->bind();
	GLint projectionMatrixIndex = glGetUniformLocation(lightingShader->getId(),"projectionMatrix");
	GLint viewMatrixIndex = glGetUniformLocation(lightingShader->getId(),"viewMatrix");
	GLint modelMatrixIndex = glGetUniformLocation(lightingShader->getId(),"modelMatrix");
	GLint cameraIndex = glGetUniformLocation(lightingShader->getId(),"cameraWorldPosition");
	GLint lightIndex = glGetUniformLocation(lightingShader->getId(),"lightPosition");
	GLint lightColorIndex = glGetUniformLocation(lightingShader->getId(),"lightColor");

#ifndef __EMSCRIPTEN__
	std::shared_ptr<Shader> normalShader = Shader::Create("mvpNormals.vert","attribColor.frag");
	if(!normalShader){
		//compiling one of the shaders failed
		waitForExit(window);
		return -1;
	}
	//attach a geometry shader
	auto gs = ShaderStage::Create(GL_GEOMETRY_SHADER);
	if(!gs->compileFromFile("ssnormals.geom")){
		printf("Compiling geometry shader failed :(");
		waitForExit(window);
		return -1;
	}
	normalShader->attach(std::move(gs));
	normalShader->bindAttrib(0,"in_Position");
	normalShader->bindAttrib(1,"in_Normal");
	normalShader->bindAttrib(2,"in_TexCoord");
	if(!normalShader->link()){
		printf("linking shader program failed :(");
		waitForExit(window);
		return -1;
	}
	//now to pass in our transform matrices we need to get the location of the uniforms
	normalShader->bind();
	GLint normalProjectionMatrixIndex = glGetUniformLocation(normalShader->getId(),"projectionMatrix");
	GLint normalViewMatrixIndex = glGetUniformLocation(normalShader->getId(),"viewMatrix");
	GLint normalModelMatrixIndex = glGetUniformLocation(normalShader->getId(),"modelMatrix");
	GLint normalLengthIndex = glGetUniformLocation(normalShader->getId(),"normalLength");
	GLint normalLightIndex = glGetUniformLocation(normalShader->getId(),"lightPosition");
#endif

	//Geometry<Plane> bb;
	IndexedGeometry<SharpCube> bb2;
	//bb.init();
	bb2.init();
	
	//pick some positions for the camera and a light
	glm::vec3 cameraPosition = glm::vec3(0.f,2.f,4.f);
	glm::vec3 lightPosition = glm::vec3(1.f,2.f,3.f);
	glm::vec3 lightColor = glm::vec3(2.f,2.f,3.f); //making the color > 1 makes the light brighter

	//We already set up a perspective projection matrix, now let's make the others
	//camera is looking toward the origin (0,0,0) with positive Y as up
	glm::mat4 viewMatrix = glm::lookAt(cameraPosition,glm::vec3(0.f,0.f,0.f),glm::vec3(0.f,1.f,0.f));
	//scale object to double it's initial size
	glm::mat4 modelMatrix = glm::scale(glm::vec3(1.5f));

	//enable depth test so that the front of the cube will occlude the back of the cube
	glEnable(GL_DEPTH_TEST);


	int counter = 0;
	//Main rendering loop
	auto main_loop = [=]() mutable {
		//first poll for events
		glfwPollEvents();

		//let's make our cube spin
		modelMatrix = glm::rotate(modelMatrix,0.5f,glm::vec3(0.f,1.f,0.f));

		//make the light move up and down
		int ipos = counter > 400 ? 800-counter : counter;
		lightPosition.y = float(ipos-200) * 0.01f;
		counter = (counter+1) % 800;

		//clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		lightingShader->bind();
		glUniformMatrix4fv(projectionMatrixIndex,1,false,glm::value_ptr(projectionMatrix));
		glUniformMatrix4fv(viewMatrixIndex,1,false,glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(modelMatrixIndex,1,false,glm::value_ptr(modelMatrix));
		glUniform3f(cameraIndex,cameraPosition.x,cameraPosition.y,cameraPosition.z);
		glUniform3f(lightIndex,lightPosition.x,lightPosition.y,lightPosition.z);
		glUniform3f(lightColorIndex,lightColor.r,lightColor.g,lightColor.b);
		//bb.draw();
		bb2.draw();
#ifndef __EMSCRIPTEN__
		normalShader->bind();
		glUniformMatrix4fv(normalProjectionMatrixIndex,1,false,glm::value_ptr(projectionMatrix));
		glUniformMatrix4fv(normalViewMatrixIndex,1,false,glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(normalModelMatrixIndex,1,false,glm::value_ptr(modelMatrix));
		glUniform1f(normalLengthIndex,0.5f);
		glUniform3f(normalLightIndex,lightPosition.x,lightPosition.y,lightPosition.z);
		//bb.draw();
		bb2.draw();
#endif

		//finally, update the screen
		glfwSwapBuffers(window);
	};

#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop_arg(&RenderLoopCallback, new std::function<void()>(main_loop), -1, false);
	onResize(window, width, height); //and call it once to set initial values
#else
	onResize(window, width, height); //and call it once to set initial values
	while (!glfwWindowShouldClose(window)) {
		main_loop();
	}
#endif
	return 0;
}