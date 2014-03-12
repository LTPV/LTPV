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
#ifndef LIB_LTPV_COMMON_H
#define LIB_LTPV_COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/time.h>
#include <time.h>

#define LTPV_DEBUG 1

typedef struct
{
    std::string taskName;
    long start;
    long stop;
} ltpv_cpu_instance;

struct ltpv_t_taskInstance
{
    long int idTask;
    char name[500];
    char *details;
    long start;
    long end;
    long ocl_queued;
    long ocl_submit;
    long ocl_size; /* must contain the data size transferred during the task, otherwise 0 */
    long ocl_bandwidth; /* bytes/s */
    struct ltpv_t_taskInstance *next;
};
typedef struct ltpv_t_taskInstance ltpv_t_taskInstance;

struct ltpv_t_task
{
    unsigned long id;
    char name[500];
    struct ltpv_t_task *next;
};
typedef struct ltpv_t_task ltpv_t_task;

struct ltpv_t_stream
{
    int id;
    char name[500];
    ltpv_t_taskInstance *firstTaskInstance;
    struct ltpv_t_stream *next;
};
typedef struct ltpv_t_stream ltpv_t_stream;

struct ltpv_t_device
{
    int id;
    char name[500];
    char *details;
    long timeOffset;
    ltpv_t_stream *firstStream;
    struct ltpv_t_device *next;
};
typedef struct ltpv_t_device ltpv_t_device;

struct ltpv_t_end_functions
{
    void (*function)();
    struct ltpv_t_end_functions *next;
};
typedef struct ltpv_t_end_functions ltpv_t_end_functions;

void ltpv_start();


void ltpv_stopAndRecord(
    const void *filename
);

long int ltpv_searchTask(const char *nameTask);
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
    char *details,
    int idStream,
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

#endif

