#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cstdio>
#include <string>
#include "infrastructure.h"

int main(int argc, char* argv[]){

	GLFWwindow* window = init(800,600,"OpenGL Window");
	if(window == nullptr){
		//initialization failed
		return -1;
	}
	//set the clear color to a nice blue :)
	glClearColor(0.1f,0.3f,0.6f,1.0f);
	//Main rendering loop
	while(!glfwWindowShouldClose(window)){
		//first poll for events
		glfwPollEvents();
		//clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		//finally, update the screen
		glfwSwapBuffers(window);
	}
	return 0;
}