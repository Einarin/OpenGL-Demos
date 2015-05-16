# OpenGL Demos in Visual Studio
A ready to go Visual Studio 2010 solution with GLEW, GLFW, & GLM and some simple OpenGL examples.
There are many OpenGL tutorials out there already. The purpose of this code base
is to provide a quick starting point for people interested in learning OpenGL,
and particularly to have a setup that builds and runs out of the box save them
the headache of setting up libraries to make a modern OpenGL context on Windows.

## Demos
### Hello World
This demo covers everything needed to draw a simple rectangle on the screen in modern OpenGL.

If anything goes wrong it will display a red image and print errors to the console

### Model View Projection
This demo shows off simple usage of the model, view, and projection matrices.

Things to try:
- render a different shape
- create an animated shape by making the geometry or matrices change

### Normals & Lighting
This demo shows the use of basic lighting.
It uses Lambertian diffuse and Blinn-Phong specular.
A geometry shader is used to draw the normal vectors and the light vector.

Things to try:
- change the animation of the cube or light or control them with the mouse
- render a different shape, observe how the lighting effects it
- add more lights
- do something cool with the geometry shader

### Advanced Lighting
implements a more advanced lighting model, pulled lots of goodies from:
- http://renderwonk.com/publications/s2010-shading-course/
- http://blog.selfshadow.com/publications/s2012-shading-course/
- http://blog.selfshadow.com/publications/s2014-shading-course/
- http://advances.realtimerendering.com/

##Licensing
This repository includes versions of GLFW, GLEW, and GLM, which are available under the terms of their own licenses.
Code written by me is made available as follows (MIT License):

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
