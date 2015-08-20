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
#include "profiling-data.hh"
#include "opencl-profiling-data.hh"
#include <CL/cl.h>
#include <stdio.h>



#define LTPV_CL_ENQUEUE 1


#define GTOF(u) {struct timeval t; gettimeofday(&t, NULL); u = t.tv_sec*1000000+t.tv_usec;}


typedef struct ltpv_t_cl_event {
    cl_event event;
} ltpv_t_cl_event;



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

