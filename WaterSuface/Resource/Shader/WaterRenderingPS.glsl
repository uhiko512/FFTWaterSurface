#version 430 core

in vec2 te_uv;
in vec3 te_normal;
in vec4 te_clipPos;
in vec3 te_worldPos;
in vec3 te_camPos;

out vec4 color;

uniform sampler2D ourTexture;
uniform sampler2D nTexture;
layout (binding = 9) uniform sampler2D reflection;
layout (binding = 10) uniform sampler2D refraction;
layout(binding = 1) uniform sampler2DArray destTex;
layout(binding = 0) uniform sampler2D destH0Tex;

void main() {
	vec3 camDir = normalize(te_camPos - te_worldPos);
	vec3 light = vec3(1000.0f, 1000.0f, 1000.0f);
	vec3 lightDir = normalize(vec3(1.0f, 0.9f, -1.0f));//normalize(light - te_worldPos);
	vec3 halfDir = normalize(camDir + lightDir); 
	
	vec3 n = normalize(vec3(texture(nTexture, te_uv)));//normalize(te_normal) * 0.02f;
	vec2 ndc = (te_clipPos.xy / te_clipPos.w) * 0.5f + 0.5f;
	float reflDistort = 3.0f;
	float refrDistort = 1.5f;
	vec3 refl = vec3(texture(reflection, vec2(clamp(ndc.x + n.x * reflDistort, 0.001f, 0.999f), clamp(-ndc.y + n.z * reflDistort, -0.999f, -0.001f))));
	vec3 refr = vec3(texture(refraction, clamp(ndc + vec2(n.x, n.z) * refrDistort, 0.001f, 0.999f)));
	vec3 col = vec3(0.0f, 0.45f, 0.3f);
	vec3 sunCol = vec3(1.0f, 0.8f, 0.5f);
	
	float f0 = 0.75f;
	float factor = f0 + (1.0f - f0) * pow(1.0f - max(dot(halfDir, camDir), 0.0f), 5.0f);
	
	//vec3 lightDir = vec3(-1.0f, -1.0f, 1.0f);
	float specular = pow(max(dot(halfDir, n), 0.0f), 1024.0f);
	float diffuse = max(dot(n, lightDir), 0.0f);
	//color = vec4(vec3(max(dot(normalize(te_normal), normalize(lightDir)), 0.0f)) * 0.5 + 0.5f, 1.0f) * mix(refl, refr, factor);
	color = vec4(specular * sunCol + mix(refr * col, refl, factor), 1.0f);
	//color = vec4(col, 1.0f);
	//color = vec4(vec3(texture(ourTexture, te_uv)), 1.0f);
	//color = vec4(vec3(texture(nTexture, te_uv)), 1.0f);
	//color = vec4(texture(destTex, vec3(te_uv, 0)).rg, 0.0f, 1.0f);
	//color = vec4(texture(destH0Tex, te_uv).rg, 0.0f, 1.0f);
}