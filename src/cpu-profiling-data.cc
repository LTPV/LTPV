#include "cpu-profiling-data.hh"
#include "profiling-data.hh"

#include <memory>
#include <atomic>
#include <sstream>
#include <algorithm>

std::map<size_t, std::vector<std::unique_ptr<ltpv_cpu_instance> > > cpu_instances_by_threads;
 
std::atomic<bool> ltpv_isCPUInit (false);
std::atomic<int> ltpv_cpu_task_index(0);

int ltpv_cpu_unqueueTaskInstances(void)
{
    size_t cpuId = (size_t)&ltpv_cpu_unqueueTaskInstances;
    size_t taskId = 0;
    ltpv_addDevice(cpuId, "CPU");

    for(auto cpu_threadIt = cpu_instances_by_threads.begin(); cpu_threadIt != cpu_instances_by_threads.end(); ++cpu_threadIt)
    {
        size_t threadId = cpu_threadIt->first;

        std::stringstream ss;
        ss << "Thread " << threadId; 
        ltpv_addStream(threadId, cpuId, ss.str().c_str());
        for(auto instanceIt = cpu_threadIt->second.begin(); instanceIt != cpu_threadIt->second.end(); ++instanceIt)
        {
            const ltpv_cpu_instance  *instance = instanceIt->get();
            const std::pair<const size_t, std::string> *task = ltpv_findTaskByName(instance->taskName);
            if (task == NULL)
            {
                taskId = ltpv_addTask(ltpv_cpu_task_index, instance->taskName.c_str());
                ltpv_cpu_task_index++;
            }
            else
            {
                taskId = task->first;
            }

            ltpv_addTaskInstance(taskId, "", "", cpuId, threadId, instance->start, instance->stop);
        }
    }
    return 0;
}

void ltpv_add_cpu_instance(const char *taskName, int threadId, long start, long stop)
{
    if (!ltpv_isCPUInit)
    {
        ltpv_isCPUInit = true;
        ltpv_add_end_functions(ltpv_cpu_unqueueTaskInstances);
    }
    std::unique_ptr<ltpv_cpu_instance> instance = std::unique_ptr<ltpv_cpu_instance> (new ltpv_cpu_instance);
    instance->taskName = std::string(taskName);
    instance->start = start;
    instance->stop = stop;

    cpu_instances_by_threads[threadId].push_back(std::move(instance));
}
