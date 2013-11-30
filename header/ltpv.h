
// macro overloading using this trick: http://stackoverflow.com/questions/3046889/optional-parameters-with-c-macros


#include <stdio.h>
#include <sys/time.h>
#include <string.h>

#ifndef LTPV_PROFILING_ON
	#define LTPV_PROFILING_ON 1
#endif
#ifndef LTPV_NUMTHREADS
	#define LTPV_NUMTHREADS 1
#endif
#ifndef LTPV_SIZEBUFFER
	#define LTPV_SIZEBUFFER 1000
#endif


#define LTPV_NUMTHREADS_T ((unsigned int)LTPV_NUMTHREADS)
#define LTPV_SIZEBUFFER_T ((unsigned int)LTPV_SIZEBUFFER)

struct ltpv_buffer_elem {
	char name[100];
	long start;
	long stop;
};
typedef struct ltpv_buffer_elem ltpv_buffer_elem;
ltpv_buffer_elem ltpv_buffer[LTPV_NUMTHREADS][LTPV_SIZEBUFFER];
unsigned int ltpv_buffer_pos[LTPV_NUMTHREADS] = {0};

//long a = (t1.tv_sec*1000000+t1.tv_usec+t2.tv_sec*1000000+t2.tv_usec);

#if LTPV_PROFILING_ON == 1
	#warning "LTPV profiling is enabled"
	#define LTPV_3ARG(ltpv_expr, ltpv_name, ltpv_idThread) do {\
		struct timeval ltpv_t1, ltpv_t2;\
		gettimeofday(&ltpv_t1, NULL);\
		ltpv_expr;\
		gettimeofday(&ltpv_t2, NULL);\
		ltpv_buffer_elem *ltpv_buf = &ltpv_buffer[ltpv_idThread][ltpv_buffer_pos[ltpv_idThread]];\
		strcpy(ltpv_buf->name, ltpv_name);\
		ltpv_buf->start = ltpv_t1.tv_sec*1000000+ltpv_t1.tv_usec;\
		ltpv_buf->stop  = ltpv_t2.tv_sec*1000000+ltpv_t2.tv_usec;\
		ltpv_buffer_pos[ltpv_idThread]++;\
	} while(0)
#else
	#define LTPV_3ARG(ltpv_expr, ltpv_name, ltpv_idThread) do {\
		ltpv_expr;\
	} while(0)
#endif



static void ltpv_end (void) __attribute__((destructor ));

static void ltpv_end(void)
{
	FILE *_null;
	_null = fopen ("/dev/null","w");
        fprintf(_null, "[LTPV] %d %d %ld %ld", LTPV_NUMTHREADS_T, LTPV_SIZEBUFFER_T, (unsigned long)ltpv_buffer, (unsigned long)ltpv_buffer_pos);
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
