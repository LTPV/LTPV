#pragma once
#include <CL/cl.h>

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

typedef struct
{
    size_t taskId;
    char name[500];
    char *details;
    size_t queue;
    cl_event *event;
    long size;
    long bandwidth;
    cl_ulong tCPU;
} ltpv_t_taskInstancesQueue;

typedef struct ltpv_t_cl_mapped
{
    void *addr;
    int size;
} ltpv_t_cl_mapped;

void ltpv_OpenCL_addMemOpTaskInstance(
    size_t memop,
    cl_command_queue queue,
    cl_event *event,
    cl_ulong tCPU,
    int size,
    const char *name = NULL,
    char *details = NULL
);
void ltpv_OpenCL_addTaskInstance(
    size_t taskId,
    cl_command_queue queue,
    cl_event *event,
    cl_ulong tCPU,
    int size,
    const char *name = NULL,
    char *details = NULL
);

int ltpv_OpenCL_unqueueTaskInstances(void);
int ltpv_OpenCL_init(void);
int ltpv_OpenCL_mapCommandQueueDevice(size_t queue, size_t idDevice);
extern "C" void ltpv_opencl_finish(void);
