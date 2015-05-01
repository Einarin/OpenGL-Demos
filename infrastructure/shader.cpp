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
#include "shader.h"
#include <fstream>
#include <iostream>

bool ShaderStage::compile(std::string source)
{
	const char* src = source.c_str();
	int len = source.size();
	glShaderSource(id,1,&src,&len);
	glCompileShader(id);
	int compiled;
	glGetShaderiv(id, GL_COMPILE_STATUS, &compiled);
	GLsizei length;
	glGetShaderiv(id,GL_INFO_LOG_LENGTH,&length);
	GLchar* buff = new GLchar[length];
	glGetShaderInfoLog(id,length,&length,buff);
	//do something with error log
	if (strlen(buff) > 0){
		std::cout << buff << std::endl;
	}
	delete[] buff;
	if (compiled != GL_TRUE)
	{
		//shader failed to compile
		return false;
	} 
	return true;
}

bool ShaderStage::compileFromFile(std::string filename){
	std::ifstream file(filename);
	if(!file.is_open()){
		return false;
	}
	std::istreambuf_iterator<char> eos;
	std::string s(std::istreambuf_iterator<char>(file), eos);
	std::cout << "compiling " << filename << std::endl;
	return compile(s);
}

bool Shader::link(){
	for(auto it = stages.begin(); it != stages.end(); it++){
		glAttachShader(id,(*it)->getId());
	}
	glLinkProgram(id);
	int linked;
		
	glGetProgramiv(id,GL_LINK_STATUS, &linked);
	GLsizei length;
	glGetProgramiv(id,GL_INFO_LOG_LENGTH,&length);
	GLchar* buff = new GLchar[length];
	glGetProgramInfoLog(id,length,&length,buff);
	//do something with error log
	if (strlen(buff) > 0){
		std::cout << buff << std::endl;
	}
	delete[] buff;
	if(linked != GL_TRUE)
	{
		//shader failed to link
		return false;
	}

	return true;
}

std::unique_ptr<Shader> Shader::Create(std::string vertShaderPath, std::string fragShaderPath){
	//first a vertex shader
	auto vertexShader = ShaderStage::Create(GL_VERTEX_SHADER);
	if(!vertexShader->compileFromFile(vertShaderPath)){
		printf("compiling vertex shader failed :(");
		return std::unique_ptr<Shader>();
	}
	//now a fragment shader
	auto fragmentShader = ShaderStage::Create(GL_FRAGMENT_SHADER);
	if(!fragmentShader->compileFromFile(fragShaderPath)){
		printf("compiling fragment shader failed :(");
		return std::unique_ptr<Shader>();
	}
	//attach them to our program
	auto shaderProgram = Shader::Create();
	shaderProgram->attach(std::move(vertexShader));
	shaderProgram->attach(std::move(fragmentShader));
	return shaderProgram;
}