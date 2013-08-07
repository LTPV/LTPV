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
#ifndef PROFILER_OCL_H
	#include <CL/cl.h>

	#define PROFILER_OCL_H

	#define PROFILER_CL_UINT        1
	#define PROFILER_CL_BOOL        2
	#define PROFILER_CL_ULONG       3
	#define PROFILER_CL_SIZE_T      4
	#define PROFILER_CL_STRING      5
	#define PROFILER_CL_DEVICE_TYPE 6
	
	#define PROFILER_CL_ENQUEUE 1

	struct profiler_t_taskInstancesQueue {
		cl_kernel kernel;
		char name[500];
		char* details;
		cl_command_queue queue;
		cl_event *event;
		int size;
		int bandwidth;
		profiler_t_taskInstancesQueue *next;
	};
	typedef struct profiler_t_taskInstancesQueue profiler_t_taskInstancesQueue;

	struct profiler_t_cl_event {
		cl_event event;
		profiler_t_cl_event *prev;
	};
	typedef struct profiler_t_cl_event profiler_t_cl_event;

	#ifdef __linux__
		#define PROFILER_CL_RED   "\033[31m"
		#define PROFILER_CL_GREEN "\033[32m"
		#define PROFILER_CL_BLUE  "\033[34m"
		#define PROFILER_CL_BOLD  "\033[1m"
		#define PROFILER_CL_UND   "\033[4m"
		#define PROFILER_CL_ENDS  "\033[0m"
		#define PROFILER_CL_CHECK(x) do { if(x!=0) { printf("%sFailed at %s:%d%s\n", PROFILER_CL_RED, __FILE__, __LINE__, PROFILER_CL_ENDS); } } while(0)
	#else
		#define PROFILER_CL_RED   ""
		#define PROFILER_CL_GREEN ""
		#define PROFILER_CL_BLUE  ""
		#define PROFILER_CL_BOLD  ""
		#define PROFILER_CL_UND   ""
		#define PROFILER_CL_ENDS  ""
		#define PROFILER_CL_CHECK(x) do { if(x!=0) { printf("Failed at %s:%d\n", __FILE__, __LINE__); } } while(0)
	#endif

	#define PROFILER_CL_DEBUG(status)\
		do {\
			if(status != CL_SUCCESS) {\
				printf("%s%s:%d ERROR:", PROFILER_CL_RED, __FILE__, __LINE__);\
				switch(status) {\
					case CL_INVALID_PROGRAM					: printf("CL_INVALID_PROGRAM")					; break;\
					case CL_INVALID_PROGRAM_EXECUTABLE		: printf("CL_INVALID_PROGRAM_EXECUTABLE")		; break;\
					case CL_INVALID_KERNEL					: printf("CL_INVALID_KERNEL")					; break;\
					case CL_INVALID_KERNEL_NAME				: printf("CL_INVALID_KERNEL_NAME")				; break;\
					case CL_INVALID_KERNEL_DEFINITION		: printf("CL_INVALID_KERNEL_DEFINITION")		; break;\
					case CL_INVALID_VALUE					: printf("CL_INVALID_VALUE")					; break;\
					case CL_INVALID_ARG_INDEX				: printf("CL_INVALID_ARG_INDEX")				; break;\
					case CL_INVALID_ARG_VALUE				: printf("CL_INVALID_ARG_VALUE")				; break;\
					case CL_INVALID_SAMPLER					: printf("CL_INVALID_SAMPLER")					; break;\
					case CL_INVALID_ARG_SIZE				: printf("CL_INVALID_ARG_SIZE")					; break;\
					case CL_INVALID_COMMAND_QUEUE			: printf("CL_INVALID_COMMAND_QUEUE")			; break;\
					case CL_INVALID_CONTEXT					: printf("CL_INVALID_CONTEXT")					; break;\
					case CL_INVALID_KERNEL_ARGS				: printf("CL_INVALID_KERNEL_ARGS")				; break;\
					case CL_INVALID_WORK_DIMENSION			: printf("CL_INVALID_WORK_DIMENSION")			; break;\
					case CL_INVALID_WORK_GROUP_SIZE			: printf("CL_INVALID_WORK_GROUP_SIZE")			; break;\
					case CL_INVALID_WORK_ITEM_SIZE			: printf("CL_INVALID_WORK_ITEM_SIZE")			; break;\
					case CL_INVALID_GLOBAL_OFFSET			: printf("CL_INVALID_GLOBAL_OFFSET")			; break;\
					case CL_INVALID_EVENT_WAIT_LIST			: printf("CL_INVALID_EVENT_WAIT_LIST")			; break;\
					case CL_OUT_OF_RESOURCES				: printf("CL_OUT_OF_RESOURCES")					; break;\
					case CL_MEM_OBJECT_ALLOCATION_FAILURE	: printf("CL_MEM_OBJECT_ALLOCATION_FAILURE")	; break;\
					case CL_INVALID_MEM_OBJECT				: printf("CL_INVALID_MEM_OBJECT")				; break;\
					case CL_MEM_COPY_OVERLAP				: printf("CL_MEM_COPY_OVERLAP")					; break;\
					case CL_OUT_OF_HOST_MEMORY				: printf("CL_OUT_OF_HOST_MEMORY")				; break;\
					case CL_PROFILING_INFO_NOT_AVAILABLE	: printf("CL_PROFILING_INFO_NOT_AVAILABLE")		; break;\
					case CL_INVALID_EVENT					: printf("CL_INVALID_EVENT")					; break;\
					default									: printf("CL_UNKNOWN")							; break;\
				}\
				printf("%s\n", PROFILER_CL_ENDS);\
			}\
		} while(0)

cl_context profiler_clCreateContext(
	cl_context_properties *properties,
	cl_uint num_devices,
	const cl_device_id *devices,
	void pfn_notify(const char *, const void *, size_t, void *),
	void *user_data,
	cl_int *errcode_ret
);

cl_command_queue profiler_clCreateCommandQueue(
	cl_context context,
	cl_device_id device,
	cl_command_queue_properties properties,
	cl_int *errcode_ret,
	const void *queueName = NULL
);

cl_kernel profiler_clCreateKernel(
	cl_program  program,
	const char *kernel_name,
	cl_int *errcode_ret
);

void profiler_cl_addTaskInstance(
	cl_kernel kernel,
	cl_command_queue queue,
	cl_event *event,
	int size = 0,
	const char* name = NULL,
	char* detail = NULL
);

cl_int profiler_clEnqueueNDRangeKernel(
	cl_command_queue command_queue,
	cl_kernel kernel,
	cl_uint work_dim,
	const size_t *global_work_offset,
	const size_t *global_work_size,
	const size_t *local_work_size,
	cl_uint num_events_in_wait_list,
	const cl_event *event_wait_list,
	cl_event *event,
	int record = 1
);

cl_int profiler_clEnqueueWriteBuffer(
	cl_command_queue command_queue,
	cl_mem buffer,
	cl_bool blocking_write,
	size_t offset,
	size_t cb,
	const void *ptr,
	cl_uint num_events_in_wait_list,
	const cl_event *event_wait_list,
	cl_event *event,
	int record = 1
);

cl_int profiler_clEnqueueReadBuffer(
	cl_command_queue command_queue,
	cl_mem buffer,
	cl_bool blocking_read,
	size_t offset,
	size_t cb,
	void *ptr,
	cl_uint num_events_in_wait_list,
	const cl_event *event_wait_list,
	cl_event *event,
	int record = 1
);

void profiler_cl_unqueueTaskInstances();

#endif
