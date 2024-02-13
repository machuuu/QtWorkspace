#version 460 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 v_FragPos;
out vec3 v_Normal;

//! [0]
void main()
{
    // Calculate vertex position in screen space
    gl_Position = projection * view * model * vec4(a_position, 1.0);

	// Calculate the fragment position in world space
	v_FragPos = vec3(model * vec4(a_position, 1.0));

	// Calculate the normal to the shape in world space
	v_Normal = vec3(inverse(transpose(model)) * vec4(a_normal, 1.0));
}
//! [0]