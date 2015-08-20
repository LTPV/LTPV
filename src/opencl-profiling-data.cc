#include "opencl-profiling-data.hh"
#include "profiling-data.hh"
#include "opencl-utils.hh"
#include <memory>
#include <vector>
#include <cstring>

std::map<size_t, size_t> ltpv_map_command_queue_device;
std::vector<std::unique_ptr<ltpv_t_taskInstancesQueue> > ltpv_taskInstancesQueue;
static size_t memop_taskid_map[LTPV_OPENCL_LAST_MEMOP] = {0};
bool ltpv_OpenCL_terminated = false;

void ltpv_opencl_finish(void)
{
    ltpv_OpenCL_unqueueTaskInstances();
    ltpv_OpenCL_terminated = true;
}

int ltpv_OpenCL_init()
{
    ltpv_add_end_functions(&ltpv_OpenCL_unqueueTaskInstances);
    memop_taskid_map[LTPV_OPENCL_READBUF_MEMOP] = ltpv_addTask(
                LTPV_OPENCL_READBUF_MEMOP,
                "Read Buffer"
            );
    memop_taskid_map[LTPV_OPENCL_WRITEBUF_MEMOP] = ltpv_addTask(
                LTPV_OPENCL_WRITEBUF_MEMOP,
                "Write Buffer"
            );
    memop_taskid_map[LTPV_OPENCL_WRITEIMG_MEMOP] = ltpv_addTask(
                LTPV_OPENCL_WRITEIMG_MEMOP,
                "Write Image"
            );
    memop_taskid_map[LTPV_OPENCL_READIMG_MEMOP] = ltpv_addTask(
                LTPV_OPENCL_READIMG_MEMOP,
                "Read Image"
            );
    memop_taskid_map[LTPV_OPENCL_MAPIMG_MEMOP] = ltpv_addTask(
                LTPV_OPENCL_MAPBUF_MEMOP,
                "Map Buffer"
            );
    memop_taskid_map[LTPV_OPENCL_UNMAP_MEMOP] = ltpv_addTask(
                LTPV_OPENCL_UNMAP_MEMOP,
                "Unmap memory object"
            );
    memop_taskid_map[LTPV_OPENCL_MAPBUF_MEMOP] = ltpv_addTask(
                LTPV_OPENCL_MAPBUF_MEMOP,
                "Map Image"
            );
    memop_taskid_map[LTPV_OPENCL_COPYIMG_MEMOP] = ltpv_addTask(
                LTPV_OPENCL_COPYIMG_MEMOP,
                "Copy Image to Image"
            );
    memop_taskid_map[LTPV_OPENCL_COPYBUF_MEMOP] = ltpv_addTask(
                LTPV_OPENCL_COPYBUF_MEMOP,
                "Copy Buffer to Buffer"
            );
    return 0;
}
void ltpv_OpenCL_addMemOpTaskInstance(
    size_t memop,
    cl_command_queue queue,
    cl_event *event,
    cl_ulong tCPU,
    int size,
    const char *name,
    char *details
)
{
    ltpv_OpenCL_addTaskInstance(memop_taskid_map[memop], queue, event, tCPU, size, name, details);
}
void ltpv_OpenCL_addTaskInstance(
    size_t taskId,
    cl_command_queue queue,
    cl_event *event,
    cl_ulong tCPU,
    int size,
    const char *name,
    char *details
)
{
    ltpv_t_taskInstancesQueue *taskInstance = new ltpv_t_taskInstancesQueue;
    taskInstance->taskId = taskId;
    strcpy(taskInstance->name, name == NULL ? "" : name);
    taskInstance->queue = (size_t) queue;
    taskInstance->event = event;
    taskInstance->size = size;
    taskInstance->details = details;
    taskInstance->tCPU = tCPU;
    ltpv_taskInstancesQueue.push_back(std::unique_ptr<ltpv_t_taskInstancesQueue> (taskInstance));
}

int ltpv_OpenCL_unqueueTaskInstances(void)
{
    cl_int err = 0;

    if (!ltpv_OpenCL_terminated)
    {
        for (auto taskInstanceIt = ltpv_taskInstancesQueue.begin();
                taskInstanceIt != ltpv_taskInstancesQueue.end(); ++taskInstanceIt)
        {
            cl_ulong queued = 0;
            cl_ulong submit = 0;
            cl_ulong start = 0;
            cl_ulong end = 0;
            size_t param_value_size_ret = 0;
            ltpv_t_taskInstancesQueue *taskInstance = taskInstanceIt->get();
            err = clWaitForEvents(1, taskInstance->event);
            LTPV_OPENCL_DEBUG(err);
            err = clGetEventProfilingInfo(
                      *(taskInstance->event), CL_PROFILING_COMMAND_QUEUED, sizeof(cl_ulong), &queued, &param_value_size_ret);
            LTPV_OPENCL_DEBUG(err);
            LTPV_OPENCL_CHECK_PARAM_VALUE_SIZE(param_value_size_ret, sizeof(cl_ulong));
            err = clGetEventProfilingInfo(
                      *(taskInstance->event), CL_PROFILING_COMMAND_SUBMIT, sizeof(cl_ulong), &submit, &param_value_size_ret);
            LTPV_OPENCL_DEBUG(err);
            LTPV_OPENCL_CHECK_PARAM_VALUE_SIZE(param_value_size_ret, sizeof(cl_ulong));
            err = clGetEventProfilingInfo(
                      *(taskInstance->event), CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &start, &param_value_size_ret);
            LTPV_OPENCL_DEBUG(err);
            LTPV_OPENCL_CHECK_PARAM_VALUE_SIZE(param_value_size_ret, sizeof(cl_ulong));
            err = clGetEventProfilingInfo(
                      *(taskInstance->event), CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &end, &param_value_size_ret);
            LTPV_OPENCL_DEBUG(err);
            LTPV_OPENCL_CHECK_PARAM_VALUE_SIZE(param_value_size_ret, sizeof(cl_ulong));
            long bandwidth = 0;

            if (taskInstance->size > 0)
            {
                float bandwidthF = (float)1000.0 * taskInstance->size / (end - start);
                bandwidth = (long)bandwidthF;
            }

            queued /= 1000;
            submit /= 1000;
            start /= 1000;
            end /= 1000;
            long offset = queued - taskInstance->tCPU;
            queued = taskInstance->tCPU;
            submit -= offset;
            start -= offset;
            end -= offset;

            if (taskInstance->taskId < LTPV_OPENCL_LAST_MEMOP) // Not kernel but transfers
            {
                queued = submit = -1;
            }

            ltpv_addTaskInstance(taskInstance->taskId, taskInstance->name, taskInstance->details,
                                 ltpv_map_command_queue_device[taskInstance->queue], taskInstance->queue, (long)start,
                                 (long)end, (long)queued, (long)submit, (long)taskInstance->size, (long)bandwidth);
        }
    }

    return 0;
}

int ltpv_OpenCL_mapCommandQueueDevice(size_t queue, size_t idDevice)
{
    ltpv_map_command_queue_device[(size_t) queue] = idDevice;
    return 0;
}
