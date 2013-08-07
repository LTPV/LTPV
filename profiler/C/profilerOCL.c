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

/*

This stack aims to provide some facilities when using the profiler with OpenCL.
How to use it? The best way to know it is to look at the basic example given with this profiler. Take a glance at callKernels.cpp in particular.

1 - Replace the following function calls:
	* clCreateContext      by profiler_clCreateContext
	* clCreateCommandQueue by profiler_clCreateCommandQueue -> add as an extra parameter a string with the queue name
	* clCreateKernel by profiler_clCreateKernel
	* cl_addTaskInstance by profiler_cl_addTaskInstance
	* clEnqueueNDRangeKernel by profiler_clEnqueueNDRangeKernel
	* clEnqueueWriteBuffer by profiler_clEnqueueWriteBuffer
	* clEnqueueReadBuffer by profiler_clEnqueueReadBuffer
	
2 - Don't forget to call profiler_start() when you want the clock to turn, and to call profiler_stopAndRecord(const void *filename) at the end to create the XML file.

3 - Compile and run your program and open index.html?p=THEPATHTOYOURXMLFILE.xml
	

*/

#include "profilerCommon.h"
#include "profilerOCL.h"

extern profiler_t_end_functions *profiler_end_functions;

profiler_t_taskInstancesQueue *profiler_taskInstancesQueue = NULL;
int profiler_cl_initialize = 0;

profiler_t_cl_event *profiler_cl_lastEvent = NULL;

inline cl_event *createEventIfPtrNull(cl_event *event, int record) {
	cl_event *eventT = event;
	if(eventT==NULL && PROFILING==ON && record==1) {
		profiler_t_cl_event *lastEvent = profiler_cl_lastEvent;
		profiler_cl_lastEvent = (profiler_t_cl_event*)malloc(sizeof(profiler_t_cl_event));
		eventT = &(profiler_cl_lastEvent->event);
		profiler_cl_lastEvent->prev = lastEvent;
	}
	return eventT;
}

cl_context profiler_clCreateContext(
	cl_context_properties *properties,
	cl_uint num_devices,
	const cl_device_id *devices,
	void pfn_notify(const char *, const void *, size_t, void *),
	void *user_data,
	cl_int *errcode_ret
) {
	cl_context contextG = clCreateContext(properties, num_devices, devices, pfn_notify, user_data, errcode_ret);
	if(PROFILING==ON) {
		if(profiler_cl_initialize==0) {
			profiler_cl_initialize=1;
			profiler_t_end_functions *newEndFunction = (profiler_t_end_functions*)malloc(sizeof(profiler_t_end_functions));
			newEndFunction->function = profiler_cl_unqueueTaskInstances;
			newEndFunction->next=NULL;
			profiler_t_end_functions ** ptrEndFunction = &profiler_end_functions;
			while(*ptrEndFunction!=NULL) {
				ptrEndFunction = &((*ptrEndFunction)->next);
			}
			*ptrEndFunction = newEndFunction;
			profiler_addTask((long)(&profiler_cl_initialize)  , "Device to Host");
			profiler_addTask((long)(&profiler_cl_initialize)+1, "Host to Device");
		}
		cl_uint nDevice = 0;
		for(nDevice=0; nDevice < num_devices; nDevice++) {
			cl_device_id device = devices[nDevice];
			
			// Synchronize clocks
			long time_offset;
			{
				struct timeval t1, t2;
		
				cl_context context;
				cl_event event;
				cl_int status;
				unsigned char X = 0;
				context = clCreateContext(NULL, 1, &device, NULL, NULL, &status); PROFILER_CL_CHECK(status);// PROFILER_CL_DEBUG(status);
				cl_command_queue queue = clCreateCommandQueue(context, device, CL_QUEUE_PROFILING_ENABLE, &status); PROFILER_CL_CHECK(status);// PROFILER_CL_DEBUG(status);
				cl_mem d_X = clCreateBuffer(context, CL_MEM_WRITE_ONLY , sizeof(unsigned char), NULL, &status); PROFILER_CL_CHECK(status);// PROFILER_CL_DEBUG(status);
				gettimeofday(&t1, NULL);
				status = clEnqueueWriteBuffer(queue, d_X, CL_FALSE, 0, sizeof(unsigned char), (const void*)&X, 0, 0, &event); PROFILER_CL_CHECK(status); PROFILER_CL_DEBUG(status);
				gettimeofday(&t2, NULL);
				clWaitForEvents(1, &event);
				long queued;
				clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_QUEUED, sizeof(cl_ulong), &queued, NULL);
				queued /= 1000;
		
				long a = (t1.tv_sec*1000000+t1.tv_usec+t2.tv_sec*1000000+t2.tv_usec);
				a /= 2;
				time_offset = a - queued;

				PROFILER_CL_CHECK(clReleaseCommandQueue(queue));
				PROFILER_CL_CHECK(clReleaseMemObject   (d_X));
				PROFILER_CL_CHECK(clReleaseContext     (context));

			}

			// Let's initiate it
			long id = (long)device; // Adress as unique identifier
			char name[400];
			clGetDeviceInfo(device, CL_DEVICE_NAME, sizeof(name), name, NULL);
			size_t len = 50000;
			char *details = (char*)malloc(sizeof(char)*len);
			details[0]='\0';
			struct infos {
				char name[200];
				int type;
				cl_command_queue_info flag;
				char help[1000];
			};
			typedef struct infos infos;

			infos infosT[]={
				{
					"CL_DEVICE_VENDOR",
					PROFILER_CL_STRING,
					CL_DEVICE_VENDOR,
					"Vendor name string."
				},
				{
					"CL_DEVICE_TYPE",
					PROFILER_CL_DEVICE_TYPE,
					CL_DEVICE_TYPE,
					"The OpenCL device type. Currently supported values are one of or a combination of: CL_DEVICE_TYPE_CPU, CL_DEVICE_TYPE_GPU, CL_DEVICE_TYPE_ACCELERATOR, or CL_DEVICE_TYPE_DEFAULT."
				},
				{
					"CL_DEVICE_ADDRESS_BITS",
					PROFILER_CL_UINT,
					CL_DEVICE_ADDRESS_BITS,
					"The default compute device address space size specified as an unsigned integer value in bits. Currently supported values are 32 or 64 bits."
				},
				{
					"CL_DEVICE_EXTENSIONS",
					PROFILER_CL_STRING,
					CL_DEVICE_EXTENSIONS,
					"Returns a list of extension names"
				},
				{
					"CL_DEVICE_VERSION",
					PROFILER_CL_STRING,
					CL_DEVICE_VERSION,
					"OpenCL version string. Returns the OpenCL version supported by the device. This version string has the following format::\nOpenCL&lt;space&gt;&lt;major_version.minor_version&gt;&lt;space&gt;&lt;vendor-specific information&gt;\nThe major_version.minor_version value returned will be 1.0."
				},
				{
					"CL_DRIVER_VERSION",
					PROFILER_CL_STRING,
					CL_DRIVER_VERSION,
					"OpenCL software driver version string in the form major_number.minor_number."
				}
			};
	
			for(unsigned int i = 0; i < sizeof(infosT)/sizeof(infos); i++) {
				strcat(details, "\t\t\t<detail>\n\t\t\t\t<name>");
				strcat(details, infosT[i].name);
				strcat(details, "</name>\n\t\t\t\t<value>");
				switch(infosT[i].type) {
					case PROFILER_CL_UINT:
						{
							cl_uint a;
							clGetDeviceInfo(device, infosT[i].flag, sizeof(a), &a, NULL);
							sprintf(&details[strlen(details)], "%d", a);
						}
						break;
					case PROFILER_CL_STRING:
						{
							size_t l = strlen(details);
							clGetDeviceInfo(device, infosT[i].flag, len-l, &details[l], NULL);
						}
						break;
					case PROFILER_CL_DEVICE_TYPE:
						{
							cl_device_type a;
							clGetDeviceInfo(device, infosT[i].flag, sizeof(a), &a, NULL);
							switch(a) {
								case CL_DEVICE_TYPE_CPU:
									strcat(details, "CL_DEVICE_TYPE_CPU");
									break;
								case CL_DEVICE_TYPE_GPU:
									strcat(details, "CL_DEVICE_TYPE_GPU");
									break;
								case CL_DEVICE_TYPE_ACCELERATOR:
									strcat(details, "CL_DEVICE_TYPE_ACCELERATOR");
									break;
								case CL_DEVICE_TYPE_DEFAULT:
									strcat(details, "CL_DEVICE_TYPE_DEFAULT");
									break;
							}
						}
						break;
				}
				//clGetDeviceInfo(device, listInfosFlag[i], 10000, &details[strlen(details)], NULL);
				strcat(details, "</value>\n");
				strcat(details, "\t\t\t\t<help>");
				strcat(details, infosT[i].help);
				strcat(details, "</help>\n");
				strcat(details, "\t\t\t</detail>\n");
			}
			profiler_addDevice(id, name, details, time_offset);
		}
	}
	return contextG;
}


cl_command_queue profiler_clCreateCommandQueue(
	cl_context context,
	cl_device_id device,
	cl_command_queue_properties properties,
	cl_int *errcode_ret,
	const void *queueName
) {
	cl_command_queue queue = clCreateCommandQueue(context, device, PROFILING*CL_QUEUE_PROFILING_ENABLE, errcode_ret);
	if(PROFILING==ON) {
		long idDevice = (long)device;
		profiler_addStream((long)queue, idDevice, queueName == NULL ? "unnamed queue" : (const char*)queueName);
	}
	return queue;
}

cl_kernel profiler_clCreateKernel(
	cl_program  program,
	const char *kernel_name,
	cl_int *errcode_ret
) {
	cl_kernel kernel = clCreateKernel(program, kernel_name, errcode_ret);
	if(PROFILING==ON) {
		long id = (long)kernel; // Adress as unique identifier
		profiler_addTask(id, kernel_name);
	}
	return kernel;
}

void profiler_cl_addTaskInstance(
	cl_kernel kernel,
	cl_command_queue queue,
	cl_event *event,
	int size,
	const char* name,
	char* details
) {
	if(PROFILING==ON) {
		profiler_t_taskInstancesQueue *taskInstance = (profiler_t_taskInstancesQueue*)malloc(sizeof(profiler_t_taskInstancesQueue));
		taskInstance->kernel = kernel;
		strcpy(taskInstance->name, name==NULL ? "" : name);
		taskInstance->queue = queue;
		taskInstance->event = event;
		taskInstance->size = size;
		taskInstance->next = NULL;
		taskInstance->details = details;
		profiler_t_taskInstancesQueue **ptrTaskInstance = &profiler_taskInstancesQueue;
		while(*ptrTaskInstance != NULL) {
			ptrTaskInstance = &((*ptrTaskInstance)->next);
		}
		*ptrTaskInstance = taskInstance;
	}
}

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
	int record
) {
	cl_event * event2 = createEventIfPtrNull(event, record);
	cl_int status = clEnqueueNDRangeKernel(command_queue, kernel, work_dim, global_work_offset, global_work_size, local_work_size, num_events_in_wait_list, event_wait_list, event2);
	if(PROFILING==ON && record==1) {
		char* details =(char*)malloc(sizeof(char)*1000);
		int i;

		strcpy(details, "\n\t\t\t\t<global_work_size>");
		for(i = 0; i < (const int)work_dim; i++) sprintf(&details[strlen(details)], "%s%ld", i>0 ? ", " : "", (long)global_work_size[i]);
		strcat(details, "</global_work_size>");
		strcat(details, "\n\t\t\t\t<local_work_size>");
		for(i = 0; i < (const int)work_dim; i++) sprintf(&details[strlen(details)], "%s%ld", i>0 ? ", " : "", (long)local_work_size[i]);
		strcat(details, "</local_work_size>");
		profiler_cl_addTaskInstance(kernel, command_queue, event2, 0, NULL, details);
	}
	return status;
}

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
	int record
) {
	cl_event * event2 = createEventIfPtrNull(event, record);
	cl_int status = clEnqueueWriteBuffer(command_queue, buffer, blocking_write, offset, cb, ptr, num_events_in_wait_list, event_wait_list, event2);
	if(PROFILING==ON && record==1) {
		profiler_cl_addTaskInstance((cl_kernel)((long)(&profiler_cl_initialize)+1), command_queue, event2, cb);
	}
	return status;
}

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
	int record
) {
	cl_event * event2 = createEventIfPtrNull(event, record);
	cl_int status = clEnqueueReadBuffer(command_queue, buffer, blocking_read, offset, cb, ptr, num_events_in_wait_list, event_wait_list, event2);
	if(PROFILING==ON && record==1) {
		profiler_cl_addTaskInstance((cl_kernel)(&profiler_cl_initialize  ), command_queue, event2, cb);
	}
	return status;
}

void profiler_cl_unqueueTaskInstances() {
	if(PROFILING==ON) {
		profiler_t_taskInstancesQueue *ptrTaskInstance = profiler_taskInstancesQueue, *ptrTaskInstanceP;
		while(ptrTaskInstance != NULL) {
			clWaitForEvents(1, ptrTaskInstance->event);
			cl_ulong queued, submitted, start, end;
			clGetEventProfilingInfo(*(ptrTaskInstance->event), CL_PROFILING_COMMAND_QUEUED, sizeof(cl_ulong), &queued   , NULL);
			clGetEventProfilingInfo(*(ptrTaskInstance->event), CL_PROFILING_COMMAND_SUBMIT, sizeof(cl_ulong), &submitted, NULL);
			clGetEventProfilingInfo(*(ptrTaskInstance->event), CL_PROFILING_COMMAND_START , sizeof(cl_ulong), &start    , NULL);
			clGetEventProfilingInfo(*(ptrTaskInstance->event), CL_PROFILING_COMMAND_END   , sizeof(cl_ulong), &end      , NULL);
			long bandwidth = 0;
			if(ptrTaskInstance->size > 0) {
				float bandwidthF = (float)1000.0*ptrTaskInstance->size/(end-start);
				bandwidth = (long)bandwidthF;
			}
		
			queued    /= 1000;
			submitted /= 1000;
			start     /= 1000;
			end       /= 1000;
			profiler_addTaskInstance((long)ptrTaskInstance->kernel, ptrTaskInstance->name, ptrTaskInstance->details, (long)ptrTaskInstance->queue, (long)queued, (long)submitted, (long)start, (long)end, (long)ptrTaskInstance->size, (long)bandwidth);
			
			ptrTaskInstanceP = ptrTaskInstance;
			ptrTaskInstance = ptrTaskInstance->next;
			free(ptrTaskInstanceP);
		}
		profiler_t_cl_event *lastEvent = profiler_cl_lastEvent;
		profiler_t_cl_event *prev;
		while(lastEvent!=NULL) {
			prev = lastEvent->prev;
			free(lastEvent);
			lastEvent = prev;
		}
		
	}
}
