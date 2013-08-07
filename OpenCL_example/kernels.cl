#include "inc/defines.h"

__kernel void FIR_GPU_dev(
		__global const unsigned char X[HEIGHT][WIDTH],
		__global float               Y[HEIGHT][WIDTH]
) {
	size_t i, j;
	i = get_global_id(1)+2;
	j = get_global_id(2)+2;
	if(j<WIDTH-2) {
		float r1;
		r1  = F12*X[i-1][j-2] + F22*X[i-1][j-1] + F23*X[i-1][j  ] + F22*X[i-1][j+1] + F12*X[i-1][j+2];
		r1 += F11*X[i-2][j-2] + F12*X[i-2][j-1] + F13*X[i-2][j  ] + F12*X[i-2][j+1] + F11*X[i-2][j+2];
		r1 += F13*X[i  ][j-2] + F23*X[i  ][j-1] + F33*X[i  ][j  ] + F23*X[i  ][j+1] + F13*X[i  ][j+2];
		r1 += F12*X[i+1][j-2] + F22*X[i+1][j-1] + F23*X[i+1][j  ] + F22*X[i+1][j+1] + F12*X[i+1][j+2];
		r1 += F11*X[i+2][j-2] + F12*X[i+2][j-1] + F13*X[i+2][j  ] + F12*X[i+2][j+1] + F11*X[i+2][j+2];
		Y[i-2][j-2]=r1;
	}
}

__kernel void SEUIL_GPU_dev(
	__global const float   X[HEIGHT][WIDTH],
	__global unsigned char Y[HEIGHT][WIDTH],
	float seuil
) {
	size_t i, j;
	i = get_global_id(1);
	j = get_global_id(2);
	if(j<WIDTH-4) {
		Y[i][j] = (unsigned char)(X[i][j] > seuil ? 0 : 255);
	}
}
