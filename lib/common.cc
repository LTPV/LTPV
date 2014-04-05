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
#include "libLTPV_common.hh"

#include <string.h>
#include <iostream>
#include <memory>
#include <atomic>
#include <sstream>
#include <algorithm>

std::map<size_t, std::unique_ptr<ltpv_t_device>> ltpv_devices;
std::map<size_t, std::string> ltpv_tasks; // TODO bimap!
std::vector<std::function<int(void)> >ltpv_end_functions;
std::map<size_t, std::vector<std::unique_ptr<ltpv_cpu_instance> > > cpu_instances_by_threads;

std::atomic<bool> ltpv_isCPUInit (false);
std::atomic<int> ltpv_cpu_task_index(0);
long ltpv_t0;

void ltpv_add_end_functions(int(*func)(void) )
{
    ltpv_end_functions.push_back(func);

}

void ltpv_start()
{
    struct timeval t;
    gettimeofday(&t, NULL);
    ltpv_t0 = t.tv_sec * 1000000 + t.tv_usec;
    return;
}
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
            auto taskIt = std::find_if (ltpv_tasks.begin(), ltpv_tasks.end(), [&] (const std::pair<size_t, std::string> p) {return ( p.second == instance->taskName);});
            if (taskIt == ltpv_tasks.end())
            {
                taskId = ltpv_addTask(ltpv_cpu_task_index, instance->taskName.c_str());
                ltpv_cpu_task_index++;
            }
            else
            {
                taskId = taskIt->first;
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

size_t ltpv_addDevice(
                    size_t idDevice,
                    const char *nameDevice,
                    const char *detailsDevice,
                    long timeOffset
                   )
{

    ltpv_t_device *newDevice = new ltpv_t_device;
    newDevice->id = idDevice;
    newDevice->name = std::string (nameDevice);
    newDevice->details = std::string (detailsDevice);
    newDevice->timeOffset = timeOffset;

    ltpv_devices[idDevice] = std::unique_ptr<ltpv_t_device> (newDevice);
    return idDevice;
}

size_t ltpv_addStream(
                    size_t idStream,
                    size_t idDevice,
                    const char *name
                   )
{
    ltpv_t_stream *newStream = new ltpv_t_stream;
    newStream->name = std::string (name);
    newStream->id = idStream;

    ltpv_devices[idDevice]->streams[idStream] = newStream;

    return idStream;
}

size_t ltpv_addTask(
                  size_t idTask,
                  const char *taskName
                 )
{
    while (ltpv_tasks.find(idTask) != ltpv_tasks.end())
    {
        idTask++;
    } // FIXME bad way to get unique index

    ltpv_tasks[idTask] = taskName;
    return idTask;
}

void ltpv_addTaskInstance(
                          size_t idTask,
                          const char *name,
                          const char *details,
                          size_t idDevice,
                          size_t idStream,
                          long start,
                          long end,
                          long ocl_queued,
                          long ocl_submit,
                          long ocl_size,
                          long ocl_bandwidth
                         )
{
    ltpv_t_taskInstance *taskInstance = new ltpv_t_taskInstance;
    taskInstance->idTask = idTask;
    taskInstance->name = std::string (name);
    taskInstance->details   = std::string(details ? details : "");
    //printf("[%ld %ld]\n", ocl_queued, start);
    taskInstance->start         = start;
    taskInstance->end           = end;
    taskInstance->ocl_queued    = ocl_queued;
    taskInstance->ocl_submit    = ocl_submit;
    taskInstance->ocl_size      = ocl_size;
    taskInstance->ocl_bandwidth = ocl_bandwidth;

    ltpv_devices[idDevice]->streams[idStream]->taskInstances.push_back(taskInstance);
}

void ltpv_stopAndRecord(
                        const char *filename
                       )
{

    // call callback functions
    for (auto func : ltpv_end_functions)
    {
        if (func)
            func();
    }

    FILE *f = fopen((const char *)filename, "w");
    fprintf(f, "<profiling>\n\t<head>\n\t\t<unit>μs</unit>\n");
    {
        // Date
        char arrDayNames[7][15] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
        char arrMonthNames[12][15] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
        time_t t = time(NULL);
        struct tm tm = *localtime(&t);
        fprintf(f, "\t<date>%s, %s %2d&lt;sup&gt;th&lt;/sup&gt; %d at &lt;b&gt;%2d:%2d:%2d&lt;/b&gt;</date>\n",
                arrDayNames[tm.tm_wday], arrMonthNames[tm.tm_mon], tm.tm_mday, tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec);
    }
    fprintf(f, "\t</head>\n");
    /* List tasks */
    fprintf(f, "\n\t<!-- List tasks -->\n");
    for (auto taskIt = ltpv_tasks.begin(); taskIt != ltpv_tasks.end(); taskIt++)
    {
        fprintf(f, "\t<task>\n\t\t<id>%ld</id>\n\t\t<name>%s</name>\n\t</task>\n", taskIt->first, taskIt->second.c_str());
    }
    /* List devices */\
        fprintf(f, "\n\t<!-- List devices -->\n");
    for (auto deviceIt = ltpv_devices.begin(); deviceIt != ltpv_devices.end(); ++deviceIt)
    {
        ltpv_t_device *device = deviceIt->second.get();
        fprintf(f, "\t<device><!-- device->id = %ld -->\n\t\t<name>%s</name>\n\t\t<details>", device->id, device->name.c_str());
        fprintf(f, "\n%s", device->details.c_str());
        fprintf(f, "</details>\n");
        for(auto streamPair : device->streams)
        {
            ltpv_t_stream *strm = streamPair.second;
            fprintf(f, "\t\t<stream>\n\t\t\t<name>%s</name>\n", strm->name.c_str());
            for (auto taskInstance : strm->taskInstances)
            {
                const char *taskName = ltpv_tasks[taskInstance->idTask].c_str();
                if (taskInstance->start     + device->timeOffset - ltpv_t0 >=
                    0) // if start < 0, it means that the task was queued before the start call
                {
                    fprintf(
                            f,
                            "\t\t\t<taskInstance>\n\t\t\t\t<idTask>%ld</idTask><!-- task.name = %s -->\n\t\t\t\t<name>%s</name>\n\t\t\t\t<start>%ld</start>\n\t\t\t\t<end>%ld</end>%s\n",
                            taskInstance->idTask,
                            taskName,
                            (taskInstance->name.empty()) ? taskName : taskInstance->name.c_str(),
                            taskInstance->start     + device->timeOffset - ltpv_t0,
                            taskInstance->end       + device->timeOffset - ltpv_t0,
                            taskInstance->details.c_str()
                           );

                    if (taskInstance->ocl_queued >= 0)
                    {
                        fprintf(f, "\t\t\t\t<ocl_queued>%ld</ocl_queued>\n", taskInstance->ocl_queued + device->timeOffset - ltpv_t0);
                    }
                    if (taskInstance->ocl_submit >= 0)
                    {
                        fprintf(f, "\t\t\t\t<ocl_submit>%ld</ocl_submit>\n", taskInstance->ocl_submit + device->timeOffset - ltpv_t0);
                    }
                    if (taskInstance->ocl_size > 0)
                    {
                        fprintf(f, "\t\t\t\t<ocl_size>%ld</ocl_size>\n", taskInstance->ocl_size);
                    }
                    if (taskInstance->ocl_size > 0)
                    {
                        fprintf(f, "\t\t\t\t<ocl_bandwidth>%ld</ocl_bandwidth>\n", taskInstance->ocl_bandwidth);
                    }
                    fprintf(f, "\t\t\t</taskInstance>\n");
                }
                else if (LTPV_DEBUG)
                {
                    printf("LTPV %s:%d: a taskInstance was seen as started before the origin and was therefore not written back: was it voluntary?\n[taskName = %s][taskInstance->start+device->timeOffset-ltpv_t0=%ld+%ld-%ld = %ld]\n",
                           __FILE__, __LINE__, taskName, taskInstance->start, device->timeOffset, ltpv_t0,
                           (taskInstance->start + device->timeOffset - ltpv_t0));
                }
            }
            fprintf(f, "\t\t</stream>\n");
        }
        fprintf(f, "\t</device>\n");
    }
    fprintf(f, "</profiling>\n");
    fclose(f);
}

