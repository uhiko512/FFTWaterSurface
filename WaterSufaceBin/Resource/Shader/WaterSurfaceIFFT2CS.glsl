#version 430 core

layout (local_size_x = 16, local_size_y = 16) in;

layout(rgba32f, binding = 1) readonly uniform image2DArray ifftTex0;
layout(rgba32f, binding = 3) readonly uniform image2DArray ifftTex1;
layout(rgba32f, binding = 4) writeonly uniform image2D dTex;
layout(rgba32f, binding = 5) writeonly uniform image2D nTex;

uniform int ifftCount;
uniform int swap;

const float N = 255.0f;

 void main() {
	float o[5];
	ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
	
	for (uint i = 0; i < ifftCount; i++) {
		float perms[] = {1.0f, -1.0f};
		int index = int(mod(int(pos.x + pos.y), 2));
		float perm = perms[index];
		
		float h = imageLoad((swap == 0 ? ifftTex0 : ifftTex1), ivec3(pos, i)).r;
		o[i] = (perm * (h / (N * N)));
	}
	
	vec3 displacement = vec3(o[0], o[1], o[2]);
	vec3 normal = normalize(vec3(-o[3], 1.0f, -o[4]));
	
	imageStore(dTex, pos, vec4(displacement, 1.0f));
	imageStore(nTex, pos, vec4(normal, 1.0f));
}
