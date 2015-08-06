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
#pragma once

#include "init.hh"
#include "common.hh"
#include <CL/cl.h>
#include <stdio.h>



#define LTPV_CL_ENQUEUE 1

#define LTPV_OPENCL_NOT_MEMOP       0
#define LTPV_OPENCL_READBUF_MEMOP   1
#define LTPV_OPENCL_WRITEBUF_MEMOP  2
#define LTPV_OPENCL_READIMG_MEMOP   3
#define LTPV_OPENCL_WRITEIMG_MEMOP  4
#define LTPV_OPENCL_MAPBUF_MEMOP    5
#define LTPV_OPENCL_UNMAP_MEMOP     6
#define LTPV_OPENCL_MAPIMG_MEMOP    7
#define LTPV_OPENCL_COPYBUF_MEMOP   8
#define LTPV_OPENCL_COPYIMG_MEMOP   9
#define LTPV_OPENCL_LAST_MEMOP      10

#define GTOF(u) {struct timeval t; gettimeofday(&t, NULL); u = t.tv_sec*1000000+t.tv_usec;}

typedef struct {
    size_t taskId;
    char name[500];
    char* details;
    size_t queue;
    cl_event *event;
    long size;
    long bandwidth;
    cl_ulong tCPU;
} ltpv_t_taskInstancesQueue;

typedef struct ltpv_t_cl_event {
    cl_event event;
} ltpv_t_cl_event;

typedef struct ltpv_t_cl_mapped {
    void *addr;
    int size;
} ltpv_t_cl_mapped;



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
                        );

cl_int clEnqueueUnmapMemObject(
                               cl_command_queue command_queue,
                               cl_mem           memobj,
                               void *           mapped_ptr,
                               cl_uint          num_events_in_wait_list,
                               const cl_event * event_wait_list,
                               cl_event *       event
                              );

// NOT IMPLEMENTED FOR NOW
//cl_int clReleaseCommandQueue(cl_command_queue command_queue);
//cl_int clReleaseContext (cl_context context);
//cl_int clReleaseDevice (cl_device_id device);
//cl_int clReleaseMemObject(cl_mem memobj);
//cl_int clReleaseSampler(cl_sampler sampler);
//cl_int clReleaseProgram(cl_program program);
//cl_int clReleaseKernel(cl_kernel kernel);
//cl_int clReleaseEvent(cl_event event);

extern "C" void ltpv_opencl_finish(void);

int ltpv_OpenCL_init(void);
int ltpv_OpenCL_unqueueTaskInstances(void);
void ltpv_OpenCL_addTaskInstance(
                                 size_t taskId,
                                 cl_command_queue queue,
                                 cl_event *event,
                                 cl_ulong tCPU,
                                 int size = 0,
                                 const char* name = NULL,
                                 char* detail = NULL
                                );
