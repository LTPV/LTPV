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

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/time.h>
#include <time.h>
#include <map>
#include <vector>
#include <functional>
#define LTPV_DEBUG 1

typedef struct
{
    std::string taskName;
    long start;
    long stop;
} ltpv_cpu_instance;

typedef struct ltpv_t_taskInstance
{
    long int idTask;
    std::string name;
    std::string details;
    long start;
    long end;
    long ocl_queued;
    long ocl_submit;
    long ocl_size; /* must contain the data size transferred during the task, otherwise 0 */
    long ocl_bandwidth; /* bytes/s */
} ltpv_t_taskInstance;

typedef struct ltpv_t_task
{
    unsigned long id;
    std::string name;
} ltpv_t_task;

typedef struct ltpv_t_stream
{
    long id;
    std::string name;
    std::vector<ltpv_t_taskInstance *> taskInstances;
} ltpv_t_stream;

typedef struct ltpv_t_device
{
    long id;
    std::string name;
    std::string details;
    long timeOffset;
    std::map<int, ltpv_t_stream *> streams;
} ltpv_t_device;

typedef struct ltpv_t_end_functions
{
    void (*function)();
    struct ltpv_t_end_functions *next;
} ltpv_t_end_functions;

void ltpv_start();


extern "C" void ltpv_add_cpu_instance(const char *taskName, int threadId, long start, long stop);

void ltpv_stopAndRecord(
    const void *filename
);

void ltpv_addDevice(
    int idDevice,
    const void *nameDevice,
    void *detailsDevice = NULL,
    long timeOffset = 0
);

void ltpv_addStream(
    int idStream,
    int idDevice,
    const void *name
);

void ltpv_addTask(
    long int idTask,
    const void *nameTask
);

void ltpv_addTaskInstance(
    long int idTask,
    const char *name,
    const char *details,
    long idDevice,
    long idStream,
    long start,
    long end,
    long ocl_queue     = -1,
    long ocl_submit    = -1,
    long ocl_size      = 0,
    long ocl_bandwidth = 0
);

void ltpv_stopAndRecord(
    const void *filename
);


void add_end_functions(std::function<int(void)> func);
