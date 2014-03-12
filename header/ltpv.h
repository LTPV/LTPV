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

void (*ltpv_addDevice_func)(int, const void *);
void (*ltpv_addStream_func)(int, int, const void *);
void (*ltpv_addTask_func)(long int, const void *);
void (*ltpv_addTaskInstance_func)(long int, const char *, char *, int, long, long);
long (*ltpv_searchTask_func)(const char *);

#define LTPV_NUMTHREADS_T ((unsigned int)LTPV_NUMTHREADS)
#define LTPV_SIZEBUFFER_T ((unsigned int)LTPV_SIZEBUFFER)
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
        ltpv_idTask = ltpv_searchTask_func(ltpv_name);\
        if (ltpv_idTask < 0){ ltpv_addTask_func(ltpv_lastTaskId, ltpv_name); ltpv_idTask = ltpv_lastTaskId++;}\
        ltpv_addTaskInstance_func(ltpv_idTask, "cpu task", "cpu", (size_t) &ltpv_addStream_func + ltpv_idThread, ltpv_t1.tv_sec*1000000+ltpv_t1.tv_usec,\
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

static void ltpv_end (void);

static void ltpv_terminate (int signum)
{
    printf("ltpv terminate : %s\n", strsignal(signum));
    ltpv_end();
    printf("ltpv ended\n");
}

static void ltpv_init(void)
{
    char *env = NULL;
    void *libLTPV = NULL;
    ltpv_addDevice_func = NULL;
    ltpv_addStream_func = NULL;
    ltpv_addTask_func = NULL;
    ltpv_addTaskInstance_func = NULL;
    ltpv_searchTask_func = NULL;

    if (!ltpv_isInit)
    {
        ltpv_isInit = true;
        env = getenv("LD_PRELOAD");
        if (env != NULL && strstr(env, "LTPV")) // FIXME dirty way
        {
            libLTPV = dlopen(env, RTLD_LAZY);
            if (dlerror() == NULL)
            {
                ltpv_addDevice_func = (void (*)(int, const void *)) dlsym(libLTPV, "ltpv_short_addDevice");
                assert(dlerror() == NULL);
                ltpv_addStream_func = (void (*)(int, int, const void *)) dlsym(libLTPV, "ltpv_addStream");
                assert(dlerror() == NULL);
                ltpv_addTask_func = (void (*)(long int, const void *)) dlsym(libLTPV, "ltpv_addTask");
                assert(dlerror() == NULL);
                ltpv_addTaskInstance_func = (void (*)(long int, const char *, char *, int, long, long)) dlsym(libLTPV,
                                            "ltpv_short_addTaskInstance");
                assert(dlerror() == NULL);
                ltpv_searchTask_func = (long (*)(const char *)) dlsym(libLTPV, "ltpv_searchTask");
                assert(dlerror() == NULL);

                ltpv_addDevice_func((size_t) &ltpv_addDevice_func /*FIXME Unique ID, dirty!*/, "CPU");
                printf("addDevice\n");
                for (unsigned int i = 0; i < LTPV_NUMTHREADS_T; ++i)
                {
                    char sname[2] = {0};
                    sname[0] = i + '0';
                    ltpv_addStream_func((size_t)&ltpv_addStream_func + i /*FIXME Unique ID, dirty!*/, (size_t) &ltpv_addDevice_func, sname);
                    printf("addStream\n");
                }
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

static void ltpv_end(void)
{
}

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





