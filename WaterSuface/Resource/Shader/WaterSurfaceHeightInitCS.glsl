#version 430 core
#define PI 3.14159265359

layout (local_size_x = 16, local_size_y = 16) in;

layout(rgba32f, binding = 0) writeonly uniform image2D destH0Tex;
 
 const float N = 255.0f;
 const float Len = 1000.0f;
 const float g = 9.81f;
 
 
 // Noise
  float rand(vec2 pos) {
	 return fract(sin(dot(pos, vec2(12.9898f, 78.233f))) * 43758.5453f);
 }
 
 float boxMuller(vec2 pos) {
	float m = 0.0f;
	float sd = 1.0f;
	return clamp((m + sd * sqrt(-2.0f * log(rand(pos))) * sin(2.0f * PI * rand(pos + vec2(N + 1.0f)))) * 0.5f + 0.5f, 0.001f, 1.0f);
 }
 
 // Spectrum 
float phillipsSpectrum(vec2 k) {
	float A = 5.0f;
	vec2 w = vec2(1.0f, 1.0f);
	float V = 200.0f;
	float L = (V * V) / g;
	float L2 = L * L;
	float lenK = max(length(k), 0.00001f);
	float lenK2 = lenK * lenK;
	float kDotw = dot(normalize(k), normalize(w));

	return A * (exp(-1.0f / (lenK2 * L2)) / (lenK2 * lenK2)) * pow(kDotw, 2);
}
 
vec2 height0(vec2 rand, vec2 k) {
	return rand * sqrt(phillipsSpectrum(k) / 2.0f);
}
 
 
void main() {
	ivec2 storePos = ivec2(gl_GlobalInvocationID.xy);
	/*vec4 noise = vec4(
		boxMuller(storePos),
		boxMuller(storePos + vec2(N + 1.0f)),
		boxMuller(storePos + vec2((N + 1.0f) * 2.0f)),
		boxMuller(storePos + vec2((N + 1.0f) * 3.0f))
	);*/
	vec4 noise = vec4(
		clamp(rand(storePos), 0.001f, 1.0f),
		clamp(rand(storePos + vec2(N + 1.0f)), 0.001f, 1.0f),
		clamp(rand(storePos + vec2((N + 1.0f) * 2.0f)), 0.001f, 1.0f),
		clamp(rand(storePos + vec2((N + 1.0f) * 3.0f)), 0.001f, 1.0f)
	);
	
	float u0 = 2.0*PI*noise.x;
	float v0 = sqrt(-2.0 * log(noise.y));
	float u1 = 2.0*PI*noise.z;
	float v1 = sqrt(-2.0 * log(noise.w));
	
	vec4 rnd = vec4(v0 * cos(u0), v0 * sin(u0), v1 * cos(u1), v1 * sin(u1));
	
	vec2 k = vec2((PI * (2.0f * storePos.x - N)) / Len, (PI * (2.0f * storePos.y - N)) / Len);
	
	vec4 h0 = vec4(
		clamp(height0(rnd.rg,  k), -4000, 4000),
		clamp(height0(rnd.ba, -k), -4000, 4000)
	);
	
	/*****/
	/*vec2 x = vec2(gl_GlobalInvocationID.xy) - float(N)/2.0;
	vec2 k = vec2(2.0 * PI * x.x/Len, 2.0 * PI * x.y/Len);
	float A = 4.0f;
	vec2 w = vec2(1.0f, 1.0f);
	float V = 40.0f;
	float L = (V * V) / g;
	float L2 = L * L;
	float lenK = max(length(k), 0.00001f);
	float lenK2 = lenK * lenK;
	float kDotw = dot(normalize(k), normalize(w));
	
	
	
	

	float L_ = (V * V)/g;
	float mag = length(k);
	if (mag < 0.00001) mag = 0.00001;
	float magSq = mag * mag;
	
	float h0k = clamp(sqrt((A/(magSq*magSq)) * pow(dot(normalize(k), normalize(w)), 8.0) * exp(-(1.0/(magSq * L_ * L_))) * exp(-magSq*pow(L/2000.0,2.0)))/ sqrt(2.0), -4000.0, 4000.0);
	float h0minusk = clamp(sqrt((A/(magSq*magSq)) * pow(dot(normalize(-k), normalize(w)), 8.0) * exp(-(1.0/(magSq * L_ * L_))) * exp(-magSq*pow(L/2000.0,2.0)))/ sqrt(2.0), -4000.0, 4000.0);
	vec4 h0 = vec4(h0k * rnd.rg, h0minusk * rnd.ba);*/
	
	imageStore(destH0Tex, storePos, h0);
}