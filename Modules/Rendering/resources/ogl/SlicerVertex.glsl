#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 modelScale;
uniform mat2 textureScale;

out vec2 vTexCoord;

void main()
{
	gl_Position = projection * view * modelScale * model * vec4(aPos,1.0);
	
	
	vTexCoord = (textureScale * (aTexCoord - vec2(0.5,0.5))) + vec2(0.5,0.5);
}