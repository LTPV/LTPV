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

#include "libLTPV_OpenCL.hh"
#define GTOF(u) {struct timeval t; gettimeofday(&t, NULL); u = t.tv_sec*1000000+t.tv_usec;}

extern ltpv_t_end_functions *ltpv_end_functions;

ltpv_t_taskInstancesQueue *ltpv_taskInstancesQueue = NULL;

int ltpv_OpenCL_initialize = 0; // The address of this variable will also be used as a unique identifier for transfers

ltpv_t_cl_event *ltpv_cl_lastEvent = NULL;

ltpv_t_cl_mapped *ltpv_cl_mapped = NULL;

// If an event was not provided, will create one for profiling reasons.
inline cl_event *ltpv_OpenCL_createEventIfPtrNull() {
	cl_event *eventT;
//	if(eventT==NULL) {
		ltpv_t_cl_event *lastEvent = ltpv_cl_lastEvent;
		ltpv_cl_lastEvent = (ltpv_t_cl_event*)malloc(sizeof(ltpv_t_cl_event));
		eventT = &(ltpv_cl_lastEvent->event);
		ltpv_cl_lastEvent->prev = lastEvent;
//	}
	return eventT;
}

cl_context clCreateContext(
	const cl_context_properties *properties,
  	cl_uint num_devices,
  	const cl_device_id *devices,
  	void (CL_CALLBACK * pfn_notify)(const char *, const void *, size_t, void *),
  	void *user_data,
  	cl_int *errcode_ret)
{
	cl_context contextG = ltpv_call_original(clCreateContext)(properties, num_devices, devices, pfn_notify, user_data, errcode_ret);

	if(ltpv_OpenCL_initialize==0) {
		ltpv_OpenCL_initialize=1;
		ltpv_t_end_functions *newEndFunction = (ltpv_t_end_functions*)malloc(sizeof(ltpv_t_end_functions));
		newEndFunction->function = ltpv_OpenCL_unqueueTaskInstances;
		newEndFunction->next=NULL;
		ltpv_t_end_functions ** ptrEndFunction = &ltpv_end_functions;
		while(*ptrEndFunction!=NULL) {
			ptrEndFunction = &((*ptrEndFunction)->next);
		}
		*ptrEndFunction = newEndFunction;
		ltpv_addTask(
			(unsigned long)(&ltpv_OpenCL_initialize),
			"Host to Device"
		);
		ltpv_addTask(
			(unsigned long)(&ltpv_OpenCL_initialize+1),
			"Device to Host"
		);
	}
	cl_uint nDevice = 0;
	for(nDevice=0; nDevice < num_devices; nDevice++) {
		cl_device_id device = devices[nDevice];
		
		// Synchronize clocks: not used anymore
		/*
		long time_offset;
		{
			struct timeval t1, t2;
	
			cl_context context;
			cl_event event;
			cl_int status;
			unsigned char X = 0;
			context = ltpv_call_original(clCreateContext)(NULL, 1, &device, NULL, NULL, &status); LTPV_OPENCL_CHECK(status);// LTPV_OPENCL_DEBUG(status);
			cl_command_queue queue = ltpv_call_original(clCreateCommandQueue)(contextG, device, CL_QUEUE_PROFILING_ENABLE, &status); LTPV_OPENCL_CHECK(status);// LTPV_OPENCL_DEBUG(status);
			cl_mem d_X = ltpv_call_original(clCreateBuffer)(contextG, CL_MEM_WRITE_ONLY , sizeof(unsigned char), NULL, &status); LTPV_OPENCL_CHECK(status);// LTPV_OPENCL_DEBUG(status);
			gettimeofday(&t1, NULL);
			status = ltpv_call_original(clEnqueueWriteBuffer)(queue, d_X, CL_FALSE, 0, sizeof(unsigned char), (const void*)&X, 0, 0, &event); LTPV_OPENCL_CHECK(status); LTPV_OPENCL_DEBUG(status);
			gettimeofday(&t2, NULL);
			clWaitForEvents(1, &event);
			long queued;
			clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_QUEUED, sizeof(cl_ulong), &queued, NULL);
			queued /= 1000;
	
			long a = (t1.tv_sec*1000000+t1.tv_usec+t2.tv_sec*1000000+t2.tv_usec);
			a /= 2;
			time_offset = a - queued;

			LTPV_OPENCL_CHECK(clReleaseCommandQueue(queue));
			LTPV_OPENCL_CHECK(clReleaseMemObject   (d_X));
			LTPV_OPENCL_CHECK(clReleaseContext     (context));
		}
		*/

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
				LTPV_OPENCL_STRING,
				CL_DEVICE_VENDOR,
				"Vendor name string."
			},
			{
				"CL_DEVICE_TYPE",
				LTPV_OPENCL_DEVICE_TYPE,
				CL_DEVICE_TYPE,
				"The OpenCL device type. Currently supported values are one of or a combination of: CL_DEVICE_TYPE_CPU, CL_DEVICE_TYPE_GPU, CL_DEVICE_TYPE_ACCELERATOR, or CL_DEVICE_TYPE_DEFAULT."
			},
			{
				"CL_DEVICE_ADDRESS_BITS",
				LTPV_OPENCL_UINT,
				CL_DEVICE_ADDRESS_BITS,
				"The default compute device address space size specified as an unsigned integer value in bits. Currently supported values are 32 or 64 bits."
			},
			{
				"CL_DEVICE_EXTENSIONS",
				LTPV_OPENCL_STRING,
				CL_DEVICE_EXTENSIONS,
				"Returns a list of extension names"
			},
			{
				"CL_DEVICE_VERSION",
				LTPV_OPENCL_STRING,
				CL_DEVICE_VERSION,
				"OpenCL version string. Returns the OpenCL version supported by the device. This version string has the following format::\nOpenCL&lt;space&gt;&lt;major_version.minor_version&gt;&lt;space&gt;&lt;vendor-specific information&gt;\nThe major_version.minor_version value returned will be 1.0."
			},
			{
				"CL_DRIVER_VERSION",
				LTPV_OPENCL_STRING,
				CL_DRIVER_VERSION,
				"OpenCL software driver version string in the form major_number.minor_number."
			}
		};

		for(unsigned int i = 0; i < sizeof(infosT)/sizeof(infos); i++) {
			strcat(details, "\t\t\t<detail>\n\t\t\t\t<name>");
			strcat(details, infosT[i].name);
			strcat(details, "</name>\n\t\t\t\t<value>");
			switch(infosT[i].type) {
				case LTPV_OPENCL_UINT:
					{
						cl_uint a;
						clGetDeviceInfo(device, infosT[i].flag, sizeof(a), &a, NULL);
						sprintf(&details[strlen(details)], "%d", a);
					}
					break;
				case LTPV_OPENCL_STRING:
					{
						size_t l = strlen(details);
						clGetDeviceInfo(device, infosT[i].flag, len-l, &details[l], NULL);
					}
					break;
				case LTPV_OPENCL_DEVICE_TYPE:
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
		//ltpv_addDevice(id, name, details, time_offset);
		// Try with an offset for each task (due to problems with an iMX6)
		ltpv_addDevice(id, name, details, 0);
	}

	return contextG;
}


cl_command_queue clCreateCommandQueue(
	cl_context context,
	cl_device_id device,
	cl_command_queue_properties properties,
	cl_int *errcode_ret
) {
	static int idQueue = 1;
	char queueName[10];
	cl_command_queue queue = ltpv_call_original(clCreateCommandQueue)(context, device, properties|CL_QUEUE_PROFILING_ENABLE, errcode_ret);

	long idDevice = (long)device;
	sprintf(queueName, "Queue %d", idQueue);
	ltpv_addStream((long)queue, idDevice, queueName);

	idQueue++;
	return queue;
}

cl_kernel clCreateKernel(
	cl_program  program,
	const char *kernel_name,
	cl_int *errcode_ret
) {
	cl_kernel kernel = ltpv_call_original(clCreateKernel)(program, kernel_name, errcode_ret);

	long id = (long)kernel; // Adress as unique identifier
	ltpv_addTask(id, kernel_name);

	return kernel;
}

void ltpv_OpenCL_addTaskInstance(
	cl_kernel kernel,
	cl_command_queue queue,
	cl_event *event,
	long tCPU,
	int size,
	const char* name,
	char* details
) {

	ltpv_t_taskInstancesQueue *taskInstance = (ltpv_t_taskInstancesQueue*)malloc(sizeof(ltpv_t_taskInstancesQueue));
	taskInstance->kernel = kernel;
	strcpy(taskInstance->name, name==NULL ? "" : name);
	taskInstance->queue = queue;
	taskInstance->event = event;
	taskInstance->size = size;
	taskInstance->next = NULL;
	taskInstance->details = details;
	taskInstance->tCPU = tCPU;
	ltpv_t_taskInstancesQueue **ptrTaskInstance = &ltpv_taskInstancesQueue;
	while(*ptrTaskInstance != NULL) {
		ptrTaskInstance = &((*ptrTaskInstance)->next);
	}
	*ptrTaskInstance = taskInstance;
}

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
) {
	cl_event *event2 = ltpv_OpenCL_createEventIfPtrNull();
	long u; GTOF(u);
	cl_int status = ltpv_call_original(clEnqueueNDRangeKernel)(command_queue, kernel, work_dim, global_work_offset, global_work_size, local_work_size, num_events_in_wait_list, event_wait_list, event2);
	if(event!=NULL)
		*event = *event2;
	char* details =(char*)malloc(sizeof(char)*1000);
	int i;

	strcpy(details, "\n\t\t\t\t<ocl_global_work_size>");
	for(i = 0; i < (const int)work_dim; i++) sprintf(&details[strlen(details)], "%s%ld", i>0 ? ", " : "", (long)global_work_size[i]);
	strcat(details, "</ocl_global_work_size>");
	strcat(details, "\n\t\t\t\t<ocl_local_work_size>");
	if(local_work_size!=NULL)
		for(i = 0; i < (const int)work_dim; i++) sprintf(&details[strlen(details)], "%s%ld", i>0 ? ", " : "", (long)local_work_size[i]);
	else
		strcat(details, "auto");
	strcat(details, "</ocl_local_work_size>");
	ltpv_OpenCL_addTaskInstance(kernel, command_queue, event2, u, 0, NULL, details);

	return status;
}

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
) {
	cl_event * event2 = ltpv_OpenCL_createEventIfPtrNull();
	long u; GTOF(u);
	cl_int status = ltpv_call_original(clEnqueueWriteBuffer)(command_queue, buffer, blocking_write, offset, cb, ptr, num_events_in_wait_list, event_wait_list, event2);
	if(event!=NULL)
		*event = *event2;
	ltpv_OpenCL_addTaskInstance((cl_kernel)&ltpv_OpenCL_initialize  , command_queue, event2, u, cb);

	return status;
}

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
) {
	cl_event * event2 = ltpv_OpenCL_createEventIfPtrNull();
	long u; GTOF(u);
	cl_int status = ltpv_call_original(clEnqueueReadBuffer)(command_queue, buffer, blocking_read, offset, cb, ptr, num_events_in_wait_list, event_wait_list, event2);
	if(event!=NULL)
		*event = *event2;
	ltpv_OpenCL_addTaskInstance((cl_kernel)(&ltpv_OpenCL_initialize+1), command_queue, event2, u, cb);
	
	return status;
}

void *clEnqueueMapBuffer(
	cl_command_queue command_queue,
	cl_mem           buffer,
	cl_bool          blocking_map, 
	cl_map_flags     map_flags,
	size_t           offset,
	size_t           cb,
	cl_uint          num_events_in_wait_list,
	const cl_event * event_wait_list,
	cl_event *       event,
	cl_int *         errcode_ret
) {
	cl_event * event2 = ltpv_OpenCL_createEventIfPtrNull();
	long u; GTOF(u);
	void *R = ltpv_call_original(clEnqueueMapBuffer)(command_queue, buffer, blocking_map, map_flags, offset, cb, num_events_in_wait_list, event_wait_list, event2, errcode_ret);
	if(event!=NULL)
		*event = *event2;
	
	ltpv_OpenCL_addTaskInstance((cl_kernel)(&ltpv_OpenCL_initialize+1), command_queue, event2, u, cb);
	ltpv_t_cl_mapped *newMap = (ltpv_t_cl_mapped *)malloc(sizeof(ltpv_t_cl_mapped));
	newMap->addr = R;
	newMap->size = cb;
	newMap->prev = ltpv_cl_mapped;
	ltpv_cl_mapped = newMap;
	return R;
}





cl_int clEnqueueUnmapMemObject(
	cl_command_queue command_queue,
	cl_mem           memobj,
	void *           mapped_ptr,
	cl_uint          num_events_in_wait_list,
	const cl_event * event_wait_list,
	cl_event *       event
) {
	cl_event * event2 = ltpv_OpenCL_createEventIfPtrNull();
	long u; GTOF(u);
	cl_int status = ltpv_call_original(clEnqueueUnmapMemObject)(command_queue, memobj, mapped_ptr, num_events_in_wait_list, event_wait_list, event2);
	if(event!=NULL)
		*event = *event2;
	int cb = 0;
	ltpv_t_cl_mapped *newMap = ltpv_cl_mapped;
	while(newMap != NULL) {
		if(newMap->addr == mapped_ptr) {
			cb = newMap->size;
			break;
		}
		newMap = newMap->prev;
	}
	ltpv_OpenCL_addTaskInstance((cl_kernel)(&ltpv_OpenCL_initialize), command_queue, event2, u, cb);

	return status;
}

cl_int clEnqueueWriteImage ( // Considered as a writeBuffer
	cl_command_queue command_queue,
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
) {
	cl_event * event2 = ltpv_OpenCL_createEventIfPtrNull();
	long u; GTOF(u);
	cl_int status = ltpv_call_original(clEnqueueWriteImage)(command_queue, image, blocking_write, origin, region, input_row_pitch, input_slice_pitch, ptr, num_events_in_wait_list, event_wait_list, event2);
	if(event!=NULL)
		*event = *event2;
	ltpv_OpenCL_addTaskInstance((cl_kernel)(&ltpv_OpenCL_initialize), command_queue, event2, u, region[1]*region[2]*input_row_pitch);

	return status;
}

void ltpv_OpenCL_unqueueTaskInstances() {
	ltpv_t_taskInstancesQueue *ptrTaskInstance = ltpv_taskInstancesQueue, *ptrTaskInstanceP;
	while(ptrTaskInstance != NULL) {
		clWaitForEvents(1, ptrTaskInstance->event);
		cl_ulong queued, submit, start, end;
		clGetEventProfilingInfo(*(ptrTaskInstance->event), CL_PROFILING_COMMAND_QUEUED, sizeof(cl_ulong), &queued   , NULL);
		clGetEventProfilingInfo(*(ptrTaskInstance->event), CL_PROFILING_COMMAND_SUBMIT, sizeof(cl_ulong), &submit   , NULL);
		clGetEventProfilingInfo(*(ptrTaskInstance->event), CL_PROFILING_COMMAND_START , sizeof(cl_ulong), &start    , NULL);
		clGetEventProfilingInfo(*(ptrTaskInstance->event), CL_PROFILING_COMMAND_END   , sizeof(cl_ulong), &end      , NULL);
		long bandwidth = 0;
		if(ptrTaskInstance->size > 0) {
			float bandwidthF = (float)1000.0*ptrTaskInstance->size/(end-start);
			bandwidth = (long)bandwidthF;
		}



		queued    /= 1000;
		submit    /= 1000;
		start     /= 1000;
		end       /= 1000;

		long offset = queued-ptrTaskInstance->tCPU;

		queued    = ptrTaskInstance->tCPU;
		submit    -= offset;
		start     -= offset;
		end       -= offset;

		if(ptrTaskInstance->kernel == (cl_kernel)((unsigned long)(&ltpv_OpenCL_initialize)  ) || ptrTaskInstance->kernel == (cl_kernel)((unsigned long)(&ltpv_OpenCL_initialize)+1)) { // Not kernel but transfers
			queued = submit = -1;
		}

		ltpv_addTaskInstance(
			(unsigned long)(ptrTaskInstance->kernel),
			ptrTaskInstance->name,
			ptrTaskInstance->details,
			(long)ptrTaskInstance->queue,
			(long)start,
			(long)end,
			(long)queued,
			(long)submit,
			(long)ptrTaskInstance->size,
			(long)bandwidth);
		ptrTaskInstanceP = ptrTaskInstance;
		ptrTaskInstance = ptrTaskInstance->next;
		free(ptrTaskInstanceP);
	}
	ltpv_t_cl_event *lastEvent = ltpv_cl_lastEvent;
	ltpv_t_cl_event *prev;
	while(lastEvent!=NULL) {
		prev = lastEvent->prev;
		free(lastEvent);
		lastEvent = prev;
	}
	return;
}
