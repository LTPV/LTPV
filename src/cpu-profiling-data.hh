#pragma once
#include <string>

typedef struct
{
    std::string taskName;
    long start;
    long stop;
} ltpv_cpu_instance;

extern "C" void ltpv_add_cpu_instance(const char *taskName, int threadId, long start, long stop);
int ltpv_cpu_unqueueTaskInstances(void);
