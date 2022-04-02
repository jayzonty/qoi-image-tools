#version 330

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexUV;

out vec2 fragUV;

uniform mat4 mvp;

void main()
{
	gl_Position = mvp * vec4(vertexPosition, 1.0);
	fragUV = vec2(vertexUV.x, 1.0 - vertexUV.y);
}
