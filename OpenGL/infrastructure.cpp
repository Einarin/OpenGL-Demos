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
#include "infrastructure.h"
#include <cstdio>
#include <string>

void glfwErrorCallback(int error, const char* description){
	printf("GLFW error %d: %s\n",error,description);
}

void APIENTRY openglErrorCallback(GLenum source,
						 GLenum type,
						 GLuint id,
						 GLenum severity,
						 GLsizei length,
						 const GLchar* message,
						 const void* userParam){
	std::string text;
	switch(severity){
	case GL_DEBUG_SEVERITY_HIGH:
		text = "High Severity ";
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		text = "Medium Severity ";
		break;
	case GL_DEBUG_SEVERITY_LOW:
		text = "Low Severity ";
		break;
	case GL_DEBUG_SEVERITY_NOTIFICATION:
		text = "Notification ";
		break;
	}
	switch(type){
	case GL_DEBUG_TYPE_ERROR:
		text += "Error: ";
		break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		text += "Deprecated: ";
		break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		text += "Undefined Behavior: ";
		break;
	case GL_DEBUG_TYPE_PORTABILITY:
		text += "Portability Warning: ";
		break;
	case GL_DEBUG_TYPE_PERFORMANCE:
		text += "Performance Warning: ";
		break;
	case GL_DEBUG_TYPE_MARKER:
		text += "Marker: ";
		break;
	case GL_DEBUG_TYPE_OTHER:
	default:
		return;
		text += "Unknown: ";
		break;
	}
	switch(source){
	case GL_DEBUG_SOURCE_SHADER_COMPILER:
		text += "Shader Compiler: ";
		break;
	default:
		break;
	}
	printf("%sid: %d: %s",text.c_str(),id,message);
	
}

void onKeyPressed(GLFWwindow* window, int key, int scancode, int action, int modifiers){
	//ESC to quit
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

GLFWwindow* init(int windowWidth, int windowHeight, const char* windowTitle){
	GLFWwindow* window;
	//set an error handling callback so we know if we have trouble initializing GLFW or OpenGL
	glfwSetErrorCallback(glfwErrorCallback);
	//intialize GLFW, which abstracts the OS specific method for intializing OpenGL and getting input
	if (!glfwInit())
        return nullptr;
	//create an OpenGL window
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT,GL_TRUE); //request an OpenGL debug context
	window = glfwCreateWindow(windowWidth,windowHeight,windowTitle,NULL,NULL);
	//no point in continuing if we can't make a window
	if(window == nullptr){
		glfwTerminate();
		return nullptr;
	}
	//In order to call OpenGL you have to make the context current on your thread first
	glfwMakeContextCurrent(window);
	//Initialize GLEW, which provides access to OpenGL extensions
	glewInit();
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		printf("glewInit failed, aborting.\n");
		glfwTerminate();
		return nullptr;
	}
	printf("Status: Using GLEW %s\n",glewGetString(GLEW_VERSION));
	//enable VSYNC so we don't get ugly tearing
	glfwSwapInterval(1);
	//set up some callbacks for input
	glfwSetKeyCallback(window,onKeyPressed);
	//print some info about the OpenGL setup
	printf("GL_RENDERER   = %s\n", (char *) glGetString(GL_RENDERER));
	printf("GL_VERSION    = %s\n", (char *) glGetString(GL_VERSION));
	printf("GL_VENDOR     = %s\n", (char *) glGetString(GL_VENDOR));
	//get the available OpenGL extensions
	std::string gl_extensions = (char *) glGetString(GL_EXTENSIONS);
	//The GL_KHR_debug extension makes handling OpenGL errors much easier
	if(gl_extensions.find("GL_KHR_debug") != std::string::npos){
		glDebugMessageCallback((GLDEBUGPROC)openglErrorCallback,nullptr);
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	} else {
		printf("Warning: this OpenGL context doesn't support debug extensions!\n");
	}
	return window;
}