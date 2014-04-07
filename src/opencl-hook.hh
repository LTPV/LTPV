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


#define LTPV_OPENCL_UINT            1
#define LTPV_OPENCL_BOOL            2
#define LTPV_OPENCL_ULONG           3
#define LTPV_OPENCL_SIZE_T          4
#define LTPV_OPENCL_STRING          5
#define LTPV_OPENCL_DEVICE_TYPE     6
#define LTPV_OPENCL_SIZE_T_ARRAY    7

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


struct ltpv_t_taskInstancesQueue {
    size_t taskId;
    char name[500];
    char* details;
    size_t queue;
    cl_event *event;
    long size;
    long bandwidth;
    long tCPU;
};
typedef struct ltpv_t_taskInstancesQueue ltpv_t_taskInstancesQueue;

struct ltpv_t_cl_event {
    cl_event event;
};
typedef struct ltpv_t_cl_event ltpv_t_cl_event;

struct ltpv_t_cl_mapped {
    void *addr;
    int size;
};
typedef struct ltpv_t_cl_mapped ltpv_t_cl_mapped;

struct infos
{
    char name[200];
    int type;
    cl_command_queue_info flag;
    char help[1000];
};
typedef struct infos infos;

const char *ltpv_commands_type_string(cl_command_type type)
{
    switch (type)
    {
        case CL_COMMAND_NDRANGE_KERNEL:                 return "Ndrange kernel";                     //CL_COMMAND_NDRANGE_KERNEL               
        case CL_COMMAND_NATIVE_KERNEL:                  return "Native kernel";                      //CL_COMMAND_NATIVE_KERNEL
        case CL_COMMAND_READ_BUFFER:                    return "Read buffer";                        //CL_COMMAND_READ_BUFFER
        case CL_COMMAND_WRITE_BUFFER:                   return "Write buffer";                       //CL_COMMAND_WRITE_BUFFER
        case CL_COMMAND_COPY_BUFFER:                    return "Copy buffer";                        //CL_COMMAND_COPY_BUFFER
        case CL_COMMAND_READ_IMAGE:                     return "Read image";                         //CL_COMMAND_READ_IMAGE
        case CL_COMMAND_WRITE_IMAGE:                    return "Write image";                        //CL_COMMAND_WRITE_IMAGE
        case CL_COMMAND_COPY_IMAGE:                     return "Copy image";                         //CL_COMMAND_COPY_IMAGE
        case CL_COMMAND_COPY_IMAGE_TO_BUFFER:           return "Copy image to buffer";               //CL_COMMAND_COPY_IMAGE_TO_BUFFER
        case CL_COMMAND_COPY_BUFFER_TO_IMAGE:           return "Copy buffer to image";               //CL_COMMAND_COPY_BUFFER_TO_IMAGE
        case CL_COMMAND_MAP_BUFFER:                     return "Map buffer";                         //CL_COMMAND_MAP_BUFFER
        case CL_COMMAND_MAP_IMAGE:                      return "Map image";                          //CL_COMMAND_MAP_IMAGE
        case CL_COMMAND_UNMAP_MEM_OBJECT:               return "Unmap mem object";                   //CL_COMMAND_UNMAP_MEM_OBJECT
        case CL_COMMAND_MARKER:                         return "Marker";                             //CL_COMMAND_MARKER
        case CL_COMMAND_ACQUIRE_GL_OBJECTS:             return "Acquire gl objects";                 //CL_COMMAND_ACQUIRE_GL_OBJECTS
        case CL_COMMAND_RELEASE_GL_OBJECTS:             return "Release gl objects";                 //CL_COMMAND_RELEASE_GL_OBJECTS
        case CL_COMMAND_READ_BUFFER_RECT:               return "Read buffer rect";                   //CL_COMMAND_READ_BUFFER_RECT
        case CL_COMMAND_WRITE_BUFFER_RECT:              return "Write buffer rect";                  //CL_COMMAND_WRITE_BUFFER_RECT
        case CL_COMMAND_COPY_BUFFER_RECT:               return "Copy buffer rect";                   //CL_COMMAND_COPY_BUFFER_RECT
        case CL_COMMAND_USER:                           return "User";                               //CL_COMMAND_USER
//        case CL_COMMAND_BARRIER:                        return "Barrier";                            //CL_COMMAND_BARRIER
//        case CL_COMMAND_MIGRATE_MEM_OBJECTS:            return "Migrate mem objects";                //CL_COMMAND_MIGRATE_MEM_OBJECTS
//        case CL_COMMAND_FILL_BUFFER:                    return "Fill buffer";                        //CL_COMMAND_FILL_BUFFER
//        case CL_COMMAND_FILL_IMAGE:                     return "Fill image";                         //CL_COMMAND_FILL_IMAGE
//        case CL_COMMAND_SVM_FREE:                       return "Svm free";                           //CL_COMMAND_SVM_FREE
//        case CL_COMMAND_SVM_MEMCPY:                     return "Svm memcpy";                         //CL_COMMAND_SVM_MEMCPY
//        case CL_COMMAND_SVM_MEMFILL:                    return "Svm memfill";                        //CL_COMMAND_SVM_MEMFILL
//        case CL_COMMAND_SVM_MAP:                        return "Svm map";                            //CL_COMMAND_SVM_MAP
//        case CL_COMMAND_SVM_UNMAP:                      return "Svm unmap";                          //CL_COMMAND_SVM_UNMAP
//        case CL_COMMAND_GL_FENCE_SYNC_OBJECT_KHR:       return "Gl fence sync object khr";           //CL_COMMAND_GL_FENCE_SYNC_OBJECT_KHR
//        case CL_COMMAND_EGL_FENCE_SYNC_OBJECT_KHR:      return "Egl fence sync object khr";          //CL_COMMAND_EGL_FENCE_SYNC_OBJECT_KHR
//        case CL_COMMAND_ACQUIRE_D3D10 OBJECTS_KHR:      return "Acquire d3d10 objects khr";          //CL_COMMAND_ACQUIRE_D3D10 OBJECTS_KHR
//        case CL_COMMAND_RELEASE_D3D10 OBJECTS_KHR:      return "Release d3d10 objects khr";          //CL_COMMAND_RELEASE_D3D10 OBJECTS_KHR
//        case CL_COMMAND_ACQUIRE_D3D11 OBJECTS_KHR:      return "Acquire d3d11 objects khr";          //CL_COMMAND_ACQUIRE_D3D11 OBJECTS_KHR
//        case CL_COMMAND_RELEASE_D3D11 OBJECTS_KHR:      return "Release d3d11 objects khr";          //CL_COMMAND_RELEASE_D3D11 OBJECTS_KHR
//        case CL_COMMAND_ACQUIRE_DX9 MEDIA_SURFACES_KHR: return "Acquire dx9 media surfaces khr";     //CL_COMMAND_ACQUIRE_DX9 MEDIA_SURFACES_KHR
//        case CL_COMMAND_RELEASE_DX9 MEDIA_SURFACES_KHR: return "Release dx9 media surfaces khr"      //CL_COMMAND_RELEASE_DX9 MEDIA_SURFACES_KHR
    }
   return "None" ;
};

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

#define GTOF(u) {struct timeval t; gettimeofday(&t, NULL); u = t.tv_sec*1000000+t.tv_usec;}


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
                                 size_t taskId,
                                 cl_command_queue queue,
                                 cl_event *event,
                                 long tCPU,
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

int ltpv_OpenCL_unqueueTaskInstances(void);
