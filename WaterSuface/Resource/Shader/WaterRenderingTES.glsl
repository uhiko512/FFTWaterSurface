#version 430 core

layout (triangles) in;

in vec2 tc_uv[];
in vec3 tc_normal[];
in vec3 tc_worldPos[];

out vec2 te_uv;
out vec3 te_normal;
out vec4 te_clipPos;
out vec3 te_worldPos;
out vec3 te_camPos;

layout (std140) uniform mat {
	mat4 view;
	mat4 invView;
	mat4 proj;
	vec3 camPos;
} camMat;

uniform float t;
uniform mat4 model;
uniform mat4 viewProj;
uniform sampler2D ourTexture;
uniform sampler2D nTexture;


void main(void){
	te_uv = gl_TessCoord.x * tc_uv[0] + gl_TessCoord.y * tc_uv[1] + gl_TessCoord.z * tc_uv[2];
	te_normal = gl_TessCoord.x * tc_normal[0] + gl_TessCoord.y * tc_normal[1] + gl_TessCoord.z * tc_normal[2];
	te_worldPos = gl_TessCoord.x * tc_worldPos[0] + gl_TessCoord.y * tc_worldPos[1] + gl_TessCoord.z * tc_worldPos[2];
	
	te_camPos = camMat.camPos;
	
	float sampleScale = 1.0f;
	
	vec2 du = vec2(1.0f / 256.0f, 0.0f);
	vec2 dv = vec2(0.0f, 1.0f / 256.0f);
	float dhu = texture(ourTexture, (te_uv + du)).r - texture(ourTexture, (te_uv - du)).r;
	float dhv = texture(ourTexture, (te_uv + dv)).r - texture(ourTexture, (te_uv - dv)).r;
	te_normal = normalize(te_normal + vec3(1.0f, 0.0f, 0.0f) * dhu + vec3(0.0f, 0.0f, 1.0f) * dhv);
	
	vec4 d = texture(ourTexture, te_uv);
	vec3 offset = vec3(d.g * -0.17f, d.r * 0.2f, d.b * -0.17f);
	te_worldPos += offset * 25.0f;
	te_clipPos = camMat.proj * camMat.view * vec4(te_worldPos, 1.0f);
	gl_Position = te_clipPos;
	//gl_Position.z += sin(length(gl_Position.xy) * 2.0f + t * 3.0f) * 3.0f;
}