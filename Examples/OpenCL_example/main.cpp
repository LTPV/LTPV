/*
Simon.DENEL.fr at Thales Research & Technology
Summer 2013

This example is a simple, pointless and perfectly unoptimized algorithm that takes as an input a picture of Lenna (http://lenna.org), and apply to it:
	* a 5 by 5 FIR (on GPU)
	* a min/max research (on CPU, but may have been done on the GPU using a divide and conquer)
	* a tresholding (on GPU again)

It would be unfair to compare the performance of this implementation with a pure CPU one: if you really want to have performance, then:
	* Put the minmax on the GPU (using two kernels)
	* Try to use pinned memory and to overlap transfers by GPU work
	* Compute more than one pixel per thread in the FIR

You can see that most of the time is spent around kernels, withotu work: it is due to the lightness of the task, unable to hide the usual latency.

*/
#include <stdio.h>
#include <stdlib.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "inc/defines.h"

#include "callKernels.hpp"

int main(int argc, char *argv[]) {
	IplImage *imgIN = 0, *imgOUT = 0; 
	uchar *data;

	int i, j;
	unsigned char *X, *Y;

	X = (unsigned char *)malloc      (sizeof(unsigned char) * HEIGHT * WIDTH);
	Y = (unsigned char *)malloc      (sizeof(unsigned char) * HEIGHT * WIDTH); // We will only use (HEIGHT-4)*(WIDTH-4) pixels, but let's allocate more for padding and intializing at the same time than X.
	if(X==NULL || Y==NULL) printf("malloc problem %s:%u\n", __FILE__, __LINE__);

	imgIN  = cvLoadImage(FILE_IN, 0); if(!imgIN) printf("Could not load image file: %s.jpg\n", FILE_IN);
	data = (uchar *)imgIN->imageData;

	// Initializing the input and put the output to 0 (easier to check if we have errors)
	for(i = 0; i < HEIGHT; i++) for(j = 0; j < WIDTH; j++) {
		X[i*WIDTH+j] = (unsigned char)data[i*imgIN->width+j];
		Y[i*WIDTH+j] = 0;
	}

	filtre(X, Y);

	imgOUT = cvCreateImage(cvSize(WIDTH-4, HEIGHT-4), IPL_DEPTH_8U, 1); if(!imgOUT) printf("Could not create imgOUT\n");
	data = (uchar *)imgOUT->imageData;
	for(i = 0; i < imgOUT->height; i++) for(j = 0; j < imgOUT->width; j++) data[i*imgOUT->widthStep+j] = Y[i*WIDTH+j];


	if(!cvSaveImage("Lenna_out.jpg", imgOUT, 0)) printf("Could not save: Lenna_out.jpg\n");

	cvReleaseImage(&imgIN );
	cvReleaseImage(&imgOUT);
	free(X);
	free(Y);


	return 0;
}
//;
