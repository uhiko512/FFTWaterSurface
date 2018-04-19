#version 430 core
#define PI 3.14159265359

layout (local_size_x = 1, local_size_y = 16) in;

layout(rgba32f, binding = 2) writeonly uniform image2D destBfTex;
layout(std430, binding = 0) buffer indices {
	int at[];
} revIndices;

const float N = 255.0f;

struct complex {
	float re;
	float im;
};

void main() {
	ivec2 storePos = ivec2(gl_GlobalInvocationID.xy);
	vec2 coord = gl_GlobalInvocationID.xy;
	float k = mod(coord.y * (N / pow(2, coord.x + 1)), N);
	complex twiddle = complex(cos(2.0f * PI * k / N), sin(2.0f * PI * k / N));
	
	int span = int(pow(2, coord.x));
	bool wing = mod(coord.y, pow(2, coord.x + 1)) < pow(2, coord.x);
	
	if (storePos.x == 0) {
		if (wing) {
			imageStore(destBfTex, storePos, vec4(
					twiddle.re,
					twiddle.im,
					revIndices.at[storePos.y],
					revIndices.at[storePos.y + 1])
				);
		} else {
			imageStore(destBfTex, storePos, vec4(
					twiddle.re,
					twiddle.im,
					revIndices.at[storePos.y - 1],
					revIndices.at[storePos.y])
				);
		}
	} else {
		if (wing) {
			imageStore(destBfTex, storePos, vec4(
					twiddle.re,
					twiddle.im,
					storePos.y,
					storePos.y + span)
				);
		} else {
			imageStore(destBfTex, storePos, vec4(
					twiddle.re,
					twiddle.im,
					storePos.y - span,
					storePos.y)
				);
		}
	}
}