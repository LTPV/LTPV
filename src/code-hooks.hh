/*
 * (C) Copyright 2013 - Simon DENEL at home
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

#include "colors.hh"
#include <dlfcn.h>
#define ltpv_call_original(x) (*((decltype(x) *) dlsym(RTLD_NEXT, #x)))

typedef struct
{
    char name[100];
    long start;
    long stop;
} ltpv_buffer_elem;


struct ltpv_t_cpu_task
{
    char name[100];
    struct ltpv_t_cpu_task *next;
};

typedef struct ltpv_t_cpu_task ltpv_t_cpu_task;


struct ltpv_t_cpu_stream
{
    long threadId;
    char name[100];
    struct ltpv_t_cpu_stream *next;
};

typedef struct ltpv_t_cpu_stream ltpv_t_cpu_stream;

static void wrap_init(void) __attribute__((constructor));
static void wrap_end(void) __attribute__((destructor));
