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
#include <memory>
#include <string>
#include <vector>
#include <gl\glew.h>

class ShaderStage {
private:
	GLuint id;
	ShaderStage(GLenum stage){
		id = glCreateShader(stage);
	}
public:
	static std::unique_ptr<ShaderStage> Create(GLenum stage){
		return std::unique_ptr<ShaderStage>(new ShaderStage(stage));
	}
	~ShaderStage(){
		glDeleteShader(id);
	}
	bool compile(std::string source);
	bool compileFromFile(std::string filename);
	int getId(){
		return id;
	}
};

class Shader {
protected:
	GLuint id;
	std::vector<std::unique_ptr<ShaderStage>> stages;
	Shader(){
		id = glCreateProgram();
	}
public:
	~Shader(){
		glDeleteProgram(id);
	}
	static std::unique_ptr<Shader> Create(){
		return std::unique_ptr<Shader>(new Shader());
	}
	static std::unique_ptr<Shader> Create(std::string vertShaderPath, std::string fragShaderPath);
	void attach(std::unique_ptr<ShaderStage> stage){
		stages.push_back(std::move(stage));
	}
	void bindAttrib(int index,std::string name){
		glBindAttribLocation(id,index, name.c_str());
	}
	bool link();
	void bind(){
		glUseProgram(id);
	}
	int getId(){
		return id;
	}
};