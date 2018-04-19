#version 430 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;

out vec2 vs_uv;
out vec3 vs_normal;
out vec3 vs_worldPos;


uniform mat4 model;

void main() {
	vs_uv = uv;
	vs_normal = vec3(0.0f, 1.0f, 0.0f);
    vs_worldPos = (model * vec4(position, 1.0f)).xyz;
}