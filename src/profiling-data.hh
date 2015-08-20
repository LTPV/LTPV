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

//#include ".hh"
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/time.h>
#include <time.h>
#include <map>
#include <vector>
#define LTPV_DEBUG 1


typedef struct ltpv_t_taskInstance
{
    size_t idTask;
    std::string name;
    std::string details;
    long start;
    long end;
    long ocl_queued;
    long ocl_submit;
    long ocl_size; /* must contain the data size transferred during the task, otherwise 0 */
    long ocl_bandwidth; /* bytes/s */
} ltpv_t_taskInstance;

#ifdef OLD
typedef struct ltpv_t_task
{
    size_t id;
    std::string name;
} ltpv_t_task;
#endif

typedef struct ltpv_t_stream
{
    size_t id;
    std::string name;
    std::vector<ltpv_t_taskInstance *> taskInstances;
} ltpv_t_stream;

typedef struct ltpv_t_device
{
    size_t id;
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

void ltpv_stopAndRecord(
    const void *filename
);

size_t ltpv_addDevice(
    size_t idDevice,
    const char *nameDevice,
    const char *detailsDevice = "",
    long timeOffset = 0
);

size_t ltpv_addStream(
    size_t idStream,
    size_t idDevice,
    const char *name
);
size_t ltpv_addTask(
    size_t idTask,
    const char *nameTask
);
const std::pair<const size_t, std::string> *ltpv_findTaskByName(const std::string& taskName);

void ltpv_addTaskInstance(
    size_t idTask,
    const char *name,
    const char *details,
    size_t idDevice,
    size_t idStream,
    long start,
    long end,
    long ocl_queue     = -1,
    long ocl_submit    = -1,
    long ocl_size      = 0,
    long ocl_bandwidth = 0
);

void ltpv_stopAndRecord(
    const char *filename
);


void ltpv_add_end_functions(int(*func)(void));
