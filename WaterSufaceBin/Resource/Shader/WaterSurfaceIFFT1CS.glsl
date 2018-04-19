#version 430 core

layout (local_size_x = 16, local_size_y = 16) in;

layout(rgba32f, binding = 2) readonly uniform image2D bfTex;
layout(rgba32f, binding = 1) uniform image2DArray ifftTex0;
layout(rgba32f, binding = 3) uniform image2DArray ifftTex1;

uniform int ifftCount;
uniform int direction;
uniform int stage;
uniform int swap;

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

 void main() {
	ivec2 storePos = ivec2(gl_GlobalInvocationID.xy);
	 
	for (uint i = 0; i < ifftCount; i++) {
		ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
		
		if (direction == 1) {
			pos = pos.yx;
		}

		vec4 bf = imageLoad(bfTex, ivec2(stage, pos.x));

		ivec3 aPos = ivec3(bf.z, pos.y, i);
		ivec3 bPos = ivec3(bf.w, pos.y, i);

		if (direction == 1) {
			aPos.xy = aPos.yx;
			bPos.xy = bPos.yx;
		}

		vec2 a = imageLoad((swap == 0 ? ifftTex0 : ifftTex1), aPos).rg;
		vec2 b = imageLoad((swap == 0 ? ifftTex0 : ifftTex1), bPos).rg;
		vec2 w = vec2(bf.x, bf.y);

		complex A = complex(a.x, a.y);
		complex B = complex(b.x, b.y);
		complex W = complex(w.x, w.y);

		complex H = add(A, mul(W, B));

		imageStore((swap == 0 ? ifftTex1 : ifftTex0), ivec3(storePos, i), vec4(H.re, H.im, 0.0f, 1.0f));
	}
}