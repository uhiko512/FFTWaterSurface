#version 450 core

layout (location = 0) in vec2 position;

layout (std140) uniform mat {
	mat4 view;
    mat4 invView;
	mat4 proj;
	vec3 camPos;
} camMat;

uniform mat4 model;

out vec3 sampleDir;

void main() {
	sampleDir = (model * camMat.invView * vec4(position, -1.0f, 1.0f)).xyz - vec3(0.0f);
    gl_Position = vec4(position, 0.0f, 1.0f);
}