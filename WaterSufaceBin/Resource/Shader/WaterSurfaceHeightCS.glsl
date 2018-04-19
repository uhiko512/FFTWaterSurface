#version 430 core
#define PI 3.14159265359

layout (local_size_x = 16, local_size_y = 16) in;

layout(rgba32f, binding = 0) readonly uniform image2D h0Tex;
layout(rgba32f, binding = 1) writeonly uniform image2DArray destTex;
 
uniform float t;

const float N = 256.0f;
const float Len = 1000.0f;
const float g = 9.81f;
 
 // Complex
 struct complex {
	 float re;
	 float im;
 };
 
 complex mul(complex a, complex b) {
	 complex num;
	 num.re = a.re * b.re - a.im * b.im;
	 num.im = a.re * b.im + a.im * b.re;
	 return num;
 }
 
 complex add(complex a, complex b) {
	 complex num;
	 num.re = a.re + b.re;
	 num.im = a.im + b.im;
	 return num;
 }
 
 // Spectrum 
float dispersion(float lenK) {
	return sqrt(g * lenK);
}
 
 
void main() {
	ivec2 storePos = ivec2(gl_GlobalInvocationID.xy);
	vec2 k = vec2(2.0f * PI * (storePos.x - (N / 2.0f)) / Len, 2.0f * PI * (storePos.y - (N / 2.0f)) / Len);

	vec4 h0T = imageLoad(h0Tex, storePos);
	complex h0[2] = {
		complex(h0T.r, h0T.g),
		complex(h0T.b, h0T.a)
	};

	float lenK = max(length(k), 0.00001f);
	
	
	float w = dispersion(lenK);
	float coswt = cos(w * t);
	float sinwt = sin(w * t);
	complex expwt[2] = {
		complex(coswt,  sinwt),
		complex(coswt, -sinwt)
	};
	
	complex h = add(mul(h0[0], expwt[0]), mul(h0[1], expwt[1]));
	vec2 ht = vec2(h.re, h.im);

	complex hx = mul(h, complex(0.0, -k.x / lenK));
	vec2 hxt = vec2(hx.re, hx.im);
	
	complex hy = mul(h, complex(0.0, -k.y / lenK));
	vec2 hyt = vec2(hy.re, hy.im);
	
	complex hnx = mul(h, complex(0.0, k.x));
	vec2 hnxt = vec2(hnx.re, hnx.im);
	
	complex hny = mul(h, complex(0.0, k.y));
	vec2 hnyt = vec2(hny.re, hny.im);
	

	imageStore(destTex, ivec3(storePos, 0), vec4(ht, 0.0, 1.0));
	imageStore(destTex, ivec3(storePos, 1), vec4(hxt, 0.0, 1.0));
	imageStore(destTex, ivec3(storePos, 2), vec4(hyt, 0.0, 1.0));
	imageStore(destTex, ivec3(storePos, 3), vec4(hnxt, 0.0, 1.0));
	imageStore(destTex, ivec3(storePos, 4), vec4(hnyt, 0.0, 1.0));
}