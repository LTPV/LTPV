#pragma once
// macro overloading using this trick: http://stackoverflow.com/questions/3046889/optional-parameters-with-c-macros

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
static bool ltpv_isInit = false;
static long ltpv_lastTaskId = 1;
//void (*prev_handler[32])(int);//SIGRTMAX

//long a = (t1.tv_sec*1000000+t1.tv_usec+t2.tv_sec*1000000+t2.tv_usec);

#if LTPV_PROFILING_ON == 1
#warning "LTPV profiling is enabled"
#define LTPV_3ARG(ltpv_expr, ltpv_name, ltpv_idThread) do {\
    if (ltpv_isInit){\
        struct timeval ltpv_t1, ltpv_t2;\
        int ltpv_idTask = -1;\
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

static void ltpv_init (void) __attribute__((constructor));
//static void ltpv_end (void) __attribute__((destructor (200)));

//static void ltpv_end (void);

//static void ltpv_terminate (int signum)
//{
//    printf("ltpv terminate : %s\n", strsignal(signum));
//    ltpv_end();
//    printf("ltpv ended\n");
//}

static void ltpv_init(void)
{
    char *env = NULL;
    void *libLTPV = NULL;

    if (!ltpv_isInit)
    {
        ltpv_isInit = true;
        env = getenv("LD_PRELOAD");
        if (env != NULL && strstr(env, "LTPV")) // FIXME dirty way
        {
            libLTPV = dlopen(env, RTLD_LAZY);
            if (dlerror() == NULL)
            {
                ltpv_add_cpu_instance_func = (void (*)(const char *, int, long, long)) dlsym(libLTPV, "ltpv_add_cpu_instance");
                assert(dlerror() == NULL);
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
    // signal(SIGABRT, &ltpv_terminate);
    // signal(SIGTERM, &ltpv_terminate);
    // signal(SIGSEGV, &ltpv_terminate);
}

//static void ltpv_end(void)
//{
//}

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





