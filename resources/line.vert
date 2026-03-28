#version 430 core

layout(location = 0) in vec3 in_pos;
layout(location = 1) in vec4 in_color;

uniform mat4 view;
uniform mat4 projection;

out vec4 vColor;

void main()
{
	vColor = in_color;
	gl_Position = projection * view * vec4(in_pos, 1.0);
}
