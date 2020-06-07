#version 430 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 uv;

out vec2 texCoordUv;
void main() {
    texCoordUv = uv;
	gl_Position = vec4(pos,1);
}