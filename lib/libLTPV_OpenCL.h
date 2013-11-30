/*
 * (C) Copyright 2013 - Thales SA (author: Simon DENEL - Thales Research & Technology)
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the GNU Lesser General Public License
 * (LGPL) version 2.1 which accompanies this distribution, and is available at
 * http://www.gnu.org/licenses/lgpl-2.1.html
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
*/

#ifndef LIB_LTPV_OPENCL_H
	#define LIB_LTPV_OPENCL_H
	
	#include <CL/cl.h>
	#include <stdio.h>
	#include "libLTPV_common.h"
	

	#define LTPV_OPENCL_UINT        1
	#define LTPV_OPENCL_BOOL        2
	#define LTPV_OPENCL_ULONG       3
	#define LTPV_OPENCL_SIZE_T      4
	#define LTPV_OPENCL_STRING      5
	#define LTPV_OPENCL_DEVICE_TYPE 6
	
	#define LTPV_CL_ENQUEUE 1

	struct ltpv_t_taskInstancesQueue {
		cl_kernel kernel;
		char name[500];
		char* details;
		cl_command_queue queue;
		cl_event *event;
		int size;
		int bandwidth;
		ltpv_t_taskInstancesQueue *next;
	};
	typedef struct ltpv_t_taskInstancesQueue ltpv_t_taskInstancesQueue;

	struct ltpv_t_cl_event {
		cl_event event;
		ltpv_t_cl_event *prev;
	};
	typedef struct ltpv_t_cl_event ltpv_t_cl_event;

	#ifdef __linux__
		#define LTPV_OPENCL_CHECK(x) do { if(x!=0) { printf("%sFailed at %s:%d%s\n", LTPV_RED, __FILE__, __LINE__, LTPV_ENDS); } } while(0)
	#else
		#define LTPV_OPENCL_CHECK(x) do { if(x!=0) { printf("Failed at %s:%d\n", __FILE__, __LINE__); } } while(0)
	#endif

	#define LTPV_OPENCL_DEBUG(status)\
		do {\
			if(status != CL_SUCCESS) {\
				printf("%s%s:%d ERROR:", LTPV_RED, __FILE__, __LINE__);\
				switch(status) {\
					case CL_INVALID_PROGRAM               : printf("CL_INVALID_PROGRAM")              ; break;\
					case CL_INVALID_PROGRAM_EXECUTABLE    : printf("CL_INVALID_PROGRAM_EXECUTABLE")   ; break;\
					case CL_INVALID_KERNEL                : printf("CL_INVALID_KERNEL")               ; break;\
					case CL_INVALID_KERNEL_NAME           : printf("CL_INVALID_KERNEL_NAME")          ; break;\
					case CL_INVALID_KERNEL_DEFINITION     : printf("CL_INVALID_KERNEL_DEFINITION")    ; break;\
					case CL_INVALID_VALUE                 : printf("CL_INVALID_VALUE")                ; break;\
					case CL_INVALID_ARG_INDEX             : printf("CL_INVALID_ARG_INDEX")            ; break;\
					case CL_INVALID_ARG_VALUE             : printf("CL_INVALID_ARG_VALUE")            ; break;\
					case CL_INVALID_SAMPLER               : printf("CL_INVALID_SAMPLER")              ; break;\
					case CL_INVALID_ARG_SIZE              : printf("CL_INVALID_ARG_SIZE")             ; break;\
					case CL_INVALID_COMMAND_QUEUE         : printf("CL_INVALID_COMMAND_QUEUE")        ; break;\
					case CL_INVALID_CONTEXT               : printf("CL_INVALID_CONTEXT")              ; break;\
					case CL_INVALID_KERNEL_ARGS           : printf("CL_INVALID_KERNEL_ARGS")          ; break;\
					case CL_INVALID_WORK_DIMENSION        : printf("CL_INVALID_WORK_DIMENSION")       ; break;\
					case CL_INVALID_WORK_GROUP_SIZE       : printf("CL_INVALID_WORK_GROUP_SIZE")      ; break;\
					case CL_INVALID_WORK_ITEM_SIZE        : printf("CL_INVALID_WORK_ITEM_SIZE")       ; break;\
					case CL_INVALID_GLOBAL_OFFSET         : printf("CL_INVALID_GLOBAL_OFFSET")        ; break;\
					case CL_INVALID_EVENT_WAIT_LIST       : printf("CL_INVALID_EVENT_WAIT_LIST")      ; break;\
					case CL_OUT_OF_RESOURCES              : printf("CL_OUT_OF_RESOURCES")             ; break;\
					case CL_MEM_OBJECT_ALLOCATION_FAILURE : printf("CL_MEM_OBJECT_ALLOCATION_FAILURE"); break;\
					case CL_INVALID_MEM_OBJECT            : printf("CL_INVALID_MEM_OBJECT")           ; break;\
					case CL_MEM_COPY_OVERLAP              : printf("CL_MEM_COPY_OVERLAP")             ; break;\
					case CL_OUT_OF_HOST_MEMORY            : printf("CL_OUT_OF_HOST_MEMORY")           ; break;\
					case CL_PROFILING_INFO_NOT_AVAILABLE  : printf("CL_PROFILING_INFO_NOT_AVAILABLE") ; break;\
					case CL_INVALID_EVENT                 : printf("CL_INVALID_EVENT")                ; break;\
					default                               : printf("CL_UNKNOWN")                      ; break;\
				}\
				printf("%s\n", LTPV_ENDS);\
			}\
		} while(0)

	cl_context clCreateContext(
		const cl_context_properties *properties,
	  	cl_uint num_devices,
	  	const cl_device_id *devices,
	  	void (CL_CALLBACK * pfn_notify)(const char *, const void *, size_t, void *),
	  	void *user_data,
	  	cl_int *errcode_ret
  	);

	cl_command_queue clCreateCommandQueue(
		cl_context context,
		cl_device_id device,
		cl_command_queue_properties properties,
		cl_int *errcode_ret
	);

	cl_kernel clCreateKernel(
		cl_program  program,
		const char *kernel_name,
		cl_int *errcode_ret
	);

	void ltpv_OpenCL_addTaskInstance(
		cl_kernel kernel,
		cl_command_queue queue,
		cl_event *event,
		int size = 0,
		const char* name = NULL,
		char* detail = NULL
	);

	cl_int clEnqueueNDRangeKernel(
		cl_command_queue command_queue,
		cl_kernel kernel,
		cl_uint work_dim,
		const size_t *global_work_offset,
		const size_t *global_work_size,
		const size_t *local_work_size,
		cl_uint num_events_in_wait_list,
		const cl_event *event_wait_list,
		cl_event *event
	);

	cl_int clEnqueueWriteBuffer(
		cl_command_queue command_queue,
		cl_mem buffer,
		cl_bool blocking_write,
		size_t offset,
		size_t cb,
		const void *ptr,
		cl_uint num_events_in_wait_list,
		const cl_event *event_wait_list,
		cl_event *event
	);
	
	cl_int clEnqueueWriteImage ( 	cl_command_queue command_queue,
		cl_mem image,
		cl_bool blocking_write,
		const size_t origin[3],
		const size_t region[3],
		size_t input_row_pitch,
		size_t input_slice_pitch,
		const void * ptr,
		cl_uint num_events_in_wait_list,
		const cl_event *event_wait_list,
		cl_event *event
	);

	cl_int clEnqueueReadBuffer(
		cl_command_queue command_queue,
		cl_mem buffer,
		cl_bool blocking_read,
		size_t offset,
		size_t cb,
		void *ptr,
		cl_uint num_events_in_wait_list,
		const cl_event *event_wait_list,
		cl_event *event
	);

	void ltpv_OpenCL_unqueueTaskInstances();
	
#endif
