#pragma once
// macro overloading using this trick: http://stackoverflow.com/questions/3046889/optional-parameters-with-c-macros

#ifdef __linux__

#include <stdio.h>
#include <sys/time.h>
#include <string.h>
#include <signal.h>
#include <stdbool.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <assert.h>

#ifndef LTPV_PROFILING_ON
#define LTPV_PROFILING_ON 1
#endif
#ifndef LTPV_NUMTHREADS
#define LTPV_NUMTHREADS 1
#endif
#ifndef LTPV_SIZEBUFFER
#define LTPV_SIZEBUFFER 1000000
#endif

static void (*ltpv_add_cpu_instance_func)(const char *, int, long, long) = NULL;
static void (*ltpv_opencl_finish)(void) = NULL;

//static void (*ltpv_end)(void) = NULL;
static bool ltpv_isInit = false;
static void ltpv_init (void) __attribute__((constructor));

static void ltpv_init(void)
{
    char *env = NULL;
    void *libLTPV = NULL;

    if (!ltpv_isInit)
    {
        ltpv_isInit = true;
        env = getenv("LD_PRELOAD");
        if (env != NULL && strstr(env, "ltpv")) // FIXME dirty way
        {
            libLTPV = dlopen(env, RTLD_LAZY);
            if (dlerror() == NULL)
            {
                ltpv_add_cpu_instance_func = (void (*)(const char *, int, long, long)) dlsym(libLTPV, "ltpv_add_cpu_instance");
                assert(dlerror() == NULL);
                ltpv_opencl_finish = (void (*)(void)) dlsym(libLTPV, "ltpv_opencl_finish");
                assert(dlerror() == NULL);
                //ltpv_end = (void (*)(void)) dlsym(libLTPV, "wrap_end");
                //assert(dlerror() == NULL);
                //dlclose(libLTPV);
            }
            else
            {
                ltpv_isInit = false;
            }
        }
        else
        {
            ltpv_isInit = false;
        }
    }
}
#endif

#define LTPV_OPENCL_FINISH() do {\
    if (ltpv_isInit && ltpv_opencl_finish != NULL)\
    {\
        ltpv_opencl_finish();\
    }\
} while (0)

#if LTPV_PROFILING_ON == 1 && defined(__linux__)
#define LTPV_3ARG(ltpv_expr, ltpv_name, ltpv_idThread) do {\
    if (ltpv_isInit){\
        struct timeval ltpv_t1, ltpv_t2;\
        gettimeofday(&ltpv_t1, NULL);\
        ltpv_expr;\
        gettimeofday(&ltpv_t2, NULL);\
        ltpv_add_cpu_instance_func(ltpv_name, ltpv_idThread, \
        ltpv_t1.tv_sec*1000000+ltpv_t1.tv_usec,\
        ltpv_t2.tv_sec*1000000+ltpv_t2.tv_usec);\
    } else ltpv_expr;\
} while(0)
#else
#define LTPV_3ARG(ltpv_expr, ltpv_name, ltpv_idThread) do {\
    ltpv_expr;\
} while(0)
#endif
// to avoid "warning: ISO C does not permit named variadic macros"
// from: http://stackoverflow.com/questions/9775284/cleaning-up-c-c-code-reveals-problems-with-variadic-macros
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wvariadic-macros"

#define LTPV_2ARG(ltpv_expr, ltpv_name) LTPV_3ARG(ltpv_expr, ltpv_name, 0)
#define LTPV_1ARG(ltpv_expr) LTPV_2ARG(ltpv_expr, #ltpv_expr)

#define GET_4TH_ARG(arg1, arg2, arg3, arg4, ...) arg4
#define LTPV_MACRO_CHOOSER(...) \
GET_4TH_ARG(__VA_ARGS__, LTPV_3ARG, \
            LTPV_2ARG, LTPV_1ARG, )

#define LTPV(...) LTPV_MACRO_CHOOSER(__VA_ARGS__)(__VA_ARGS__)

#pragma GCC diagnostic pop
