#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>

#include "inc/defines.h"
#include "inc/colors.h"
#include "inc/utilities.h"
#include "callKernels.hpp"

#include <CL/cl.h>
#define PLATFORM 0
#define DEVICE 0
#define FILENAME "kernels.cl"

#include "../profiler/C/profiler.h"

using namespace std;

void filtre(unsigned char *X, unsigned char *Y) {
	struct timeval t1, t2;
	
	profiler_generic_addDevice("CPU");
	 profiler_generic_addStream("Workload");
	  int idMinMax = profiler_generic_addTask("MinMax");
	
	
	// Selecting the platform
	cout << "Available platforms" << endl;
	cl_platform_id platform=0;
	cl_uint nPlatforms=0; cout << RED << flush; CHECK(clGetPlatformIDs(0, NULL, &nPlatforms)); cout << ENDS << flush;
	cl_platform_id *platforms = 0; platforms = new cl_platform_id[nPlatforms]; CHECKUNEQ(platforms, 0);
	CHECK(clGetPlatformIDs(nPlatforms, platforms, NULL));
	for(unsigned int i = 0; i < nPlatforms; i++) {
		char name[100]; CHECK(clGetPlatformInfo( platforms[i], CL_PLATFORM_NAME, sizeof(name), name, NULL));
		cout << "[" << i << "] " << name; 
		if(i==PLATFORM) {
			cout << BOLD << " [SELECTED]" << ENDS;
			platform = platforms[i];
			//break;
		}
		cout << endl;
	}
	delete[] platforms;
	
	// Selecting the device
	cout << "Available devices" << endl;
	cl_device_id device;
	cl_uint nDevices; CHECK(clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 0, NULL, &nDevices));
	cl_device_id *devices = 0; devices = new cl_device_id[nPlatforms]; CHECKUNEQ(devices, 0);
	CHECK(clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, nDevices, devices, NULL));
	for(unsigned int i = 0; i < nDevices; i++) {
		char name[100]; CHECK(clGetDeviceInfo( devices[i], CL_DEVICE_NAME, sizeof(name), name, NULL));
		cout << "[" << i << "] " << name;
		if(i==DEVICE) {
			cout << BOLD << " [SELECTED]" << ENDS << endl;
			device = devices[i];
		}
		cout << endl;
	}
	delete[] devices;

	// Creating the context
	cl_context context;
	cl_int status;
	context = profiler_clCreateContext(NULL, 1, &device, NULL, NULL, &status); CHECK(status);

	// Loading the kernels
	char * fKernels = 0;
	unsigned long length;
	FILE * f = fopen(FILENAME, "rb");
	CHECKUNEQ(f, 0);
	fseek(f, 0, SEEK_END);
	length = ftell(f);
	fseek(f, 0, SEEK_SET);
	fKernels=(char*)malloc(length); CHECKUNEQ(fKernels, 0);
	CHECKEQ(fread(fKernels, 1, length, f), length);
	fclose(f);
	
	// Compiling it
	cl_program program;
	program = clCreateProgramWithSource(context, 1, (const char**)&fKernels, (const size_t *)&length, &status); CHECK(status);
	free(fKernels);
	status = clBuildProgram(program, 0, NULL, "", NULL, NULL);
	if (status != CL_SUCCESS) { // displaying LOG
		CERR("Impossible to build the program");
		size_t len; 	char buffer[65536];
		buffer[65535] = '\0';
		clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
		printf("BUILD LOG\n-----------------\n");
		fprintf(stderr, "%s\n", buffer);
	}
	
	// Creating command queues
	cl_command_queue queueDTH, queueHTD, queueW;
	queueDTH = profiler_clCreateCommandQueue(context, device, 0, &status, "Device to host"); CHECK(status);
	queueHTD = profiler_clCreateCommandQueue(context, device, 0, &status, "Host to Device"); CHECK(status);
	queueW   = profiler_clCreateCommandQueue(context, device, 0, &status,       "Workload"); CHECK(status);

	// Creating device buffers
	cl_mem Xd, Ytd, Yd;
	Xd  = clCreateBuffer(context, CL_MEM_READ_ONLY , sizeof(unsigned char)*HEIGHT*WIDTH, NULL, &status); CHECK(status);
	Ytd = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(float        )*HEIGHT*WIDTH, NULL, &status); CHECK(status);
	Yd  = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(unsigned char)*HEIGHT*WIDTH, NULL, &status); CHECK(status);
	float *Yt = (float*)malloc(sizeof(float        )*HEIGHT*WIDTH);
	gettimeofday(&t1, NULL);
	
	// Creating events
	cl_event events[500];
	int cntEvents=0;

	// Create and init kernels
	cl_kernel FIR_GPU_dev;
	FIR_GPU_dev   = profiler_clCreateKernel(program, "FIR_GPU_dev", &status); CHECK(status); //DEBUG(status);
	CHECK(clSetKernelArg(FIR_GPU_dev, 0, sizeof(unsigned char *), &Xd ));
	CHECK(clSetKernelArg(FIR_GPU_dev, 1, sizeof(float         *), &Ytd));


	cl_kernel SEUIL_GPU_dev;
	SEUIL_GPU_dev = profiler_clCreateKernel(program, "SEUIL_GPU_dev", &status); CHECK(status); //DEBUG(status);
	CHECK(clSetKernelArg(SEUIL_GPU_dev, 0, sizeof(float        *), &Ytd));
	CHECK(clSetKernelArg(SEUIL_GPU_dev, 1, sizeof(unsigned char*), &Yd ));

	size_t global_work_size[3] = {1, HEIGHT-4, WIDTH};
	size_t  local_work_size[3] = {1,        1,    64};

	// Let's do the real work!
	profiler_start(); // This function will start the chronometer. You can call it after having created devices, kernels and queues...
	for(int loop = 0; loop <50; loop++) {
		// Launching FIR on GPU
		{
			// Transferring X from host to device
			CHECK(profiler_clEnqueueWriteBuffer(queueHTD, Xd, CL_FALSE, 0, WIDTH*HEIGHT*sizeof(unsigned char), X, 0, NULL, &events[cntEvents]));
			cntEvents++;

			// Start FIR
			status = profiler_clEnqueueNDRangeKernel(queueW, FIR_GPU_dev, 3, NULL, global_work_size, local_work_size, 1, &events[cntEvents-1], &events[cntEvents]); CHECK(status);// DEBUG(status);
			cntEvents++;

			// Transfer Yt back to host
			CHECK(profiler_clEnqueueReadBuffer(queueDTH, Ytd, CL_FALSE, 0, WIDTH*HEIGHT*sizeof(float), Yt, 1, &events[cntEvents-1], &events[cntEvents]));
			clWaitForEvents(1, &events[cntEvents]);
			cntEvents++;
		}

		// Launching MinMax
		profiler_generic_taskInstanceStart();
		float min, max;
		min = max = Yt[0];
		for(int i = 0; i < HEIGHT-4; i++) for(int j = 0; j < WIDTH-4; j++) {
			float r = Yt[i*WIDTH+j];
			if(min > r) min =r;
			if(max < r) max =r;
		}
		float seuil = 0.06*(max-min);
		profiler_generic_addTaskInstance(idMinMax);
	
		// Launching SEUIL
		{
			//for(int i = 0; i < HEIGHT-4; i++) for(int j = 0; j < WIDTH-4; j++) Y[i*WIDTH+j] = Yt[i*WIDTH+j]>seuil ? 0 : 255;

			// Yt is already in the GPU
			CHECK(clSetKernelArg(SEUIL_GPU_dev  , 2, sizeof(float), &seuil));
			CHECK(profiler_clEnqueueNDRangeKernel(queueW, SEUIL_GPU_dev, 3, NULL, global_work_size, local_work_size, 0, NULL, &events[cntEvents]));
			cntEvents++;

			CHECK(profiler_clEnqueueReadBuffer(queueDTH, Yd, CL_FALSE, 0, WIDTH*HEIGHT*sizeof(unsigned char), Y, 1, &events[cntEvents-1], &events[cntEvents]));
			clWaitForEvents(1, &events[cntEvents-1]);
			cntEvents++;

		}

	}
	clFinish(queueDTH); clFinish(queueHTD); clFinish(queueW);
	gettimeofday(&t2, NULL);

	cout << GREEN << "execution time = " << (1000000*t2.tv_sec+t2.tv_usec-1000000*t1.tv_sec-t1.tv_usec) << "us" << ENDS << endl;
	
	// Cleaning
	CHECK(clReleaseCommandQueue(queueDTH));
	CHECK(clReleaseCommandQueue(queueHTD));
	CHECK(clReleaseCommandQueue(queueW  ));
	CHECK(clReleaseMemObject(Xd));
	CHECK(clReleaseMemObject(Ytd));
	CHECK(clReleaseMemObject(Yd));
	CHECK(clReleaseProgram(program));
	CHECK(clReleaseContext(context));
	free(Yt);
	
	profiler_stopAndRecord("../profiler/profiling.xml");
}
