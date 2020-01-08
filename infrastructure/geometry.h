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
#pragma once
#include <GL/glew.h>
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include <cstring>

template<class T>
class Geometry {
protected:
	GLuint vao;
	GLuint vbo;
public:
	void init(){
		glGenVertexArrays(1, &vao);
		glGenBuffers(1,&vbo);
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER,vbo);
		size_t vbuffsize = T::vertexBufferSize();
		char* vbuff = new char[vbuffsize];
		T::tesselate(vbuff);
		glBufferData(GL_ARRAY_BUFFER,vbuffsize,vbuff,GL_STATIC_DRAW);
		T::configureAttributes();
		glBindVertexArray(0);
	}
	void draw(){
		glBindVertexArray(vao);
		glDrawArrays(T::getPrimitiveType(),0,T::getVertexCount());
		glBindVertexArray(0);
	}
};

template<class T>
class IndexedGeometry {
protected:
	GLuint vao;
	GLuint vbo;
	GLuint ibo;
public:
	void init(){
		glGenVertexArrays(1,&vao);
		glGenBuffers(1,&vbo);
		glGenBuffers(1, &ibo);
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER,vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		size_t vbuffsize = T::vertexBufferSize();
		size_t ibuffsize = T::indexBufferSize();
		char* vbuff = new char[vbuffsize];
		char* ibuff = new char[ibuffsize];
		T::tesselate(vbuff,ibuff);
		glBufferData(GL_ARRAY_BUFFER,vbuffsize,vbuff,GL_STATIC_DRAW);
		T::configureAttributes();
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,ibuffsize,ibuff,GL_STATIC_DRAW);
		glBindVertexArray(0);
	}
	void draw(){
		glBindVertexArray(vao);
		glDrawElements(T::getPrimitiveType(),T::getElementCount(),T::getIndexType(),0);
		glBindVertexArray(0);
	}
};

class Billboard{
public:
	static size_t vertexBufferSize(){
		return 16*sizeof(float);
	}
	static void tesselate(char* vertexBuffer){
		const float verts[] = {
		//  xpos  ypos  xtex ytex
			-1.0, -1.0, 0.0, 0.0,
			 1.0, -1.0, 1.0, 0.0,
			-1.0,  1.0, 0.0, 1.0,
			 1.0,  1.0, 1.0, 1.0
		};
		//memcpy(vertexBuffer,verts,16*sizeof(float));
		float* fbuffer = (float*)vertexBuffer;
		for(int i=0;i<16;i++){
			fbuffer[i] = verts[i];
		}
	}
	static void configureAttributes(){
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,4*sizeof(float),0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,4*sizeof(float),(GLvoid*)(2*sizeof(float)));
	}
	static GLenum getPrimitiveType(){
		return GL_TRIANGLE_STRIP;
	}
	static GLuint getVertexCount(){
		return 4;
	}
};

class Plane{
public:
	static size_t vertexBufferSize(){
		return 4 * 8 *sizeof(float);
	}
	static void tesselate(char* vertexBuffer){
		const float verts[] = {
		//  xpos  ypos zpos  xn   yn   zn   xtex ytex
			-1.0, 0.f, -1.0, 0.f, 1.f, 0.f, 0.0, 0.0,
			 1.0, 0.f, -1.0, 0.f, 1.f, 0.f, 1.0, 0.0,
			-1.0, 0.f,  1.0, 0.f, 1.f, 0.f, 0.0, 1.0,
			 1.0, 0.f,  1.0, 0.f, 1.f, 0.f, 1.0, 1.0
		};
		//memcpy(vertexBuffer,verts,16*sizeof(float));
		float* fbuffer = (float*)vertexBuffer;
		for(int i=0;i<32;i++){
			fbuffer[i] = verts[i];
		}
	}
	static void configureAttributes(){
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,8*sizeof(float),0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,8*sizeof(float),(GLvoid*)(3*sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,8*sizeof(float),(GLvoid*)(6*sizeof(float)));
	}
	static GLenum getPrimitiveType(){
		return GL_TRIANGLE_STRIP;
	}
	static GLuint getVertexCount(){
		return 4;
	}
};

class SharpCube{
public:
	static size_t vertexBufferSize(){
		return 24 * 8 * sizeof(float);//6 * Plane::vertexBufferSize();
	}
	static size_t indexBufferSize(){
		return 6*6;
	}
	static size_t getElementCount(){
		return indexBufferSize();
	}
	static GLenum getPrimitiveType(){
		return GL_TRIANGLES;
	}
	static GLenum getIndexType(){
		return GL_UNSIGNED_BYTE;
	}
	static void configureAttributes(){
		//Plane::configureAttributes();
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,8*sizeof(float),0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,8*sizeof(float),(GLvoid*)(5*sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,8*sizeof(float),(GLvoid*)(3*sizeof(float)));
	}
	static void tesselate(char* vertexBuffer, char* indexBuffer){
		const float g_cube[24][8] =
		{
			// Positive Z Face.
			{ -1.0f, -1.0f,  1.0f,  0.0f, 0.0f,  0.0f, 0.0f, 1.0f},
			{  1.0f, -1.0f,  1.0f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f},
			{  1.0f,  1.0f,  1.0f,  1.0f, 1.0f,  0.0f, 0.0f, 1.0f},
			{ -1.0f,  1.0f,  1.0f,  0.0f, 1.0f,  0.0f, 0.0f, 1.0f},

			// Negative Z Face.
			{  1.0f, -1.0f, -1.0f,  0.0f, 0.0f,  0.0f, 0.0f, -1.0f},
			{ -1.0f, -1.0f, -1.0f,  1.0f, 0.0f,  0.0f, 0.0f, -1.0f},
			{ -1.0f,  1.0f, -1.0f,  1.0f, 1.0f,  0.0f, 0.0f, -1.0f},
			{  1.0f,  1.0f, -1.0f,  0.0f, 1.0f,  0.0f, 0.0f, -1.0f},

			// Positive Y Face.
			{ -1.0f,  1.0f,  1.0f,  0.0f, 0.0f,  0.0f, 1.0f, 0.0f},
			{  1.0f,  1.0f,  1.0f,  1.0f, 0.0f,  0.0f, 1.0f, 0.0f},
			{  1.0f,  1.0f, -1.0f,  1.0f, 1.0f,  0.0f, 1.0f, 0.0f},
			{ -1.0f,  1.0f, -1.0f,  0.0f, 1.0f,  0.0f, 1.0f, 0.0f},

			// Negative Y Face.
			{ -1.0f, -1.0f, -1.0f,  0.0f, 0.0f,  0.0f, -1.0f, 0.0f},
			{  1.0f, -1.0f, -1.0f,  1.0f, 0.0f,  0.0f, -1.0f, 0.0f},
			{  1.0f, -1.0f,  1.0f,  1.0f, 1.0f,  0.0f, -1.0f, 0.0f},
			{ -1.0f, -1.0f,  1.0f,  0.0f, 1.0f,  0.0f, -1.0f, 0.0f},

			// Positive X Face.
			{  1.0f, -1.0f,  1.0f,  0.0f, 0.0f,  1.0f, 0.0f, 0.0f},
			{  1.0f, -1.0f, -1.0f,  1.0f, 0.0f,  1.0f, 0.0f, 0.0f},
			{  1.0f,  1.0f, -1.0f,  1.0f, 1.0f,  1.0f, 0.0f, 0.0f},
			{  1.0f,  1.0f,  1.0f,  0.0f, 1.0f,  1.0f, 0.0f, 0.0f},

			// Negative X Face.
			{ -1.0f, -1.0f, -1.0f,  0.0f, 0.0f,  -1.0f, 0.0f, 0.0f},
			{ -1.0f, -1.0f,  1.0f,  1.0f, 0.0f,  -1.0f, 0.0f, 0.0f},
			{ -1.0f,  1.0f,  1.0f,  1.0f, 1.0f,  -1.0f, 0.0f, 0.0f},
			{ -1.0f,  1.0f, -1.0f,  0.0f, 1.0f,  -1.0f, 0.0f, 0.0f}
		};
		memcpy(vertexBuffer,g_cube,sizeof(g_cube));
		/*Plane::tesselate(vertexBuffer);
		glm::mat4 transforms[6];
		transforms[0] = glm::mat4();
		transforms[1] = glm::rotate(transforms[0],180.f,glm::vec3(1.f,0.f,0.f));
		transforms[2] = glm::rotate(transforms[0],90.f,glm::vec3(1.f,0.f,0.f));
		transforms[3] = glm::rotate(transforms[0],270.f,glm::vec3(1.f,0.f,0.f));
		transforms[4] = glm::rotate(transforms[0],90.f,glm::vec3(0.f,0.f,1.f));
		transforms[5] = glm::rotate(transforms[0],270.f,glm::vec3(0.f,0.f,1.f));
		float* fbuff = (float*)vertexBuffer;
		fbuff[1] = 1.f;
		fbuff[9] = 1.f;
		fbuff[17] = 1.f;
		fbuff[25] = 1.f;
		for(int f=1;f<6;f++){
			float* baseindex = fbuff+(9*f);
			for(int v=0;v<4;v++){
				float* index = baseindex+(3*v);
				float* vbuff = fbuff + (3*v);
				glm::vec3 pos(vbuff[0],vbuff[1],vbuff[2]);
				glm::vec3 norm(vbuff[3],vbuff[4],vbuff[5]);
				glm::vec4 tpos = transforms[0] * glm::vec4(pos,1.f);
				glm::vec4 tnorm = transforms[0] * glm::vec4(norm,0.f);
				index[0] = tpos.x;
				index[1] = tpos.y;
				index[2] = tpos.z;
				index[3] = tnorm.x;
				index[4] = tnorm.y;
				index[5] = tnorm.z;
				index[6] = vbuff[6];
				index[7] = vbuff[7];
			}
		}*/
		GLubyte indices[] = {0, 1, 2, 2, 0, 3};
		for(GLubyte f=0;f<6;f++){
			for(GLubyte i=0;i<6;i++){
				((GLubyte*)indexBuffer)[6*f+i] = indices[i] + (4*f);
			}
		}
	}
};
