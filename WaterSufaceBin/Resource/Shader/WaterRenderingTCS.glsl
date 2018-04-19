#version 430 core

layout (vertices = 3) out;

in vec2 vs_uv[];
in vec3 vs_normal[];
in vec3 vs_worldPos[];

out vec2 tc_uv[];
out vec3 tc_normal[];
out vec3 tc_worldPos[];

void main(void){
	float level = 256.0f;
    if (gl_InvocationID == 0){
        gl_TessLevelInner[0] = level;
        gl_TessLevelOuter[0] = level;
        gl_TessLevelOuter[1] = level;
        gl_TessLevelOuter[2] = level;
    } 
	
    tc_worldPos[gl_InvocationID] = vs_worldPos[gl_InvocationID];
	tc_uv[gl_InvocationID] = vs_uv[gl_InvocationID];
	tc_normal[gl_InvocationID] = vs_normal[gl_InvocationID];
}