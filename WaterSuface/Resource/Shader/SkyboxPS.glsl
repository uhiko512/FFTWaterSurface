#version 450 core

layout(binding = 8) uniform samplerCube skybox;

in vec3 sampleDir;

out vec4 color;

void main() {
	color = texture(skybox, sampleDir);
}
