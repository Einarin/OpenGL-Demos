#version 330
layout(triangles) in;
layout(line_strip, max_vertices = 12) out;
in vec3 vs_WorldPosition[];
in vec3 vs_WorldNormal[];
in vec3 vs_EyeVector[];
in vec3 vs_LightVector[];

out vec4 color;

uniform float normalLength;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main()
{
	for(int i=0; i < 3; i++)
	{
		vec4 worldPosition = vec4(vs_WorldPosition[i],1.0);
		//Normal - Light Blue
		color = vec4(0.0,0.6,1.0,1.0);
		gl_Position = gl_in[i].gl_Position;
		EmitVertex();
		gl_Position = projectionMatrix * viewMatrix * (worldPosition + normalLength * vec4(vs_WorldNormal[i].xyz,0.));
		EmitVertex();
		EndPrimitive();
        //Light - Yellow
        color = vec4(0.9,1.0,0.0,1.0);
		gl_Position = gl_in[i].gl_Position;
		EmitVertex();
		gl_Position = projectionMatrix * viewMatrix * (worldPosition + normalLength * vec4(vs_LightVector[i].xyz,0.));
		EmitVertex();
		EndPrimitive();
	}
}