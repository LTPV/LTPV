#pragma once
#include <CL/cl.h>

#define LTPV_OPENCL_UINT            1
#define LTPV_OPENCL_BOOL            2
#define LTPV_OPENCL_ULONG           3
#define LTPV_OPENCL_SIZE_T          4
#define LTPV_OPENCL_STRING          5
#define LTPV_OPENCL_DEVICE_TYPE     6
#define LTPV_OPENCL_SIZE_T_ARRAY    7

#ifdef __linux__
#define LTPV_OPENCL_CHECK(x) do { if(x!=0) { printf("%sFailed at %s:%d%s\n", LTPV_RED, __FILE__, __LINE__, LTPV_ENDS); } } while(0)
#define LTPV_OPENCL_CHECK_PARAM_VALUE_SIZE(param_value_size_ret, type_size) do { if(param_value_size_ret != type_size) { printf("%sERROR: incorrect parameter value size returned. Expected %lu, Got %lu at %s:%d%s\n", LTPV_RED, type_size, param_value_size_ret, __FILE__, __LINE__, LTPV_ENDS); } } while(0)

#else
#define LTPV_OPENCL_CHECK(x) do { if(x!=0) { printf("Failed at %s:%d\n", __FILE__, __LINE__); } } while(0)
#define LTPV_OPENCL_CHECK_PARAM_VALUE_SIZE(param_value_size_ret, type_size) do { if(param_value_size_ret != type_size) { printf("ERROR: incorrect parameter value size returned.  Expected %u, Got %u at %s:%d\n", type_size,  param_value_size_ret,  __FILE__, __LINE__); } } while(0)
#endif

#define LTPV_OPENCL_DEBUG(status)\
do {\
    if(status != CL_SUCCESS) {\
        printf("%s%s:%d ERROR:", LTPV_RED, __FILE__, __LINE__);\
        switch(status) {\
            case CL_INVALID_PROGRAM               : printf("CL_INVALID_PROGRAM")              ; break;\
            case CL_INVALID_PROGRAM_EXECUTABLE    : printf("CL_INVALID_PROGRAM_EXECUTABLE")   ; break;\
            case CL_INVALID_KERNEL                : printf("CL_INVALID_KERNEL")               ; break;\
            case CL_INVALID_KERNEL_NAME           : printf("CL_INVALID_KERNEL_NAME")          ; break;\
            case CL_INVALID_KERNEL_DEFINITION     : printf("CL_INVALID_KERNEL_DEFINITION")    ; break;\
            case CL_INVALID_VALUE                 : printf("CL_INVALID_VALUE")                ; break;\
            case CL_INVALID_ARG_INDEX             : printf("CL_INVALID_ARG_INDEX")            ; break;\
            case CL_INVALID_ARG_VALUE             : printf("CL_INVALID_ARG_VALUE")            ; break;\
            case CL_INVALID_SAMPLER               : printf("CL_INVALID_SAMPLER")              ; break;\
            case CL_INVALID_ARG_SIZE              : printf("CL_INVALID_ARG_SIZE")             ; break;\
            case CL_INVALID_COMMAND_QUEUE         : printf("CL_INVALID_COMMAND_QUEUE")        ; break;\
            case CL_INVALID_CONTEXT               : printf("CL_INVALID_CONTEXT")              ; break;\
            case CL_INVALID_KERNEL_ARGS           : printf("CL_INVALID_KERNEL_ARGS")          ; break;\
            case CL_INVALID_WORK_DIMENSION        : printf("CL_INVALID_WORK_DIMENSION")       ; break;\
            case CL_INVALID_WORK_GROUP_SIZE       : printf("CL_INVALID_WORK_GROUP_SIZE")      ; break;\
            case CL_INVALID_WORK_ITEM_SIZE        : printf("CL_INVALID_WORK_ITEM_SIZE")       ; break;\
            case CL_INVALID_GLOBAL_OFFSET         : printf("CL_INVALID_GLOBAL_OFFSET")        ; break;\
            case CL_INVALID_EVENT_WAIT_LIST       : printf("CL_INVALID_EVENT_WAIT_LIST")      ; break;\
            case CL_OUT_OF_RESOURCES              : printf("CL_OUT_OF_RESOURCES")             ; break;\
            case CL_MEM_OBJECT_ALLOCATION_FAILURE : printf("CL_MEM_OBJECT_ALLOCATION_FAILURE"); break;\
            case CL_INVALID_MEM_OBJECT            : printf("CL_INVALID_MEM_OBJECT")           ; break;\
            case CL_MEM_COPY_OVERLAP              : printf("CL_MEM_COPY_OVERLAP")             ; break;\
            case CL_OUT_OF_HOST_MEMORY            : printf("CL_OUT_OF_HOST_MEMORY")           ; break;\
            case CL_PROFILING_INFO_NOT_AVAILABLE  : printf("CL_PROFILING_INFO_NOT_AVAILABLE") ; break;\
            case CL_INVALID_EVENT                 : printf("CL_INVALID_EVENT")                ; break;\
            default                               : printf("CL_UNKNOWN")                      ; break;\
        }\
        printf("%s\n", LTPV_ENDS);\
    }\
} while(0)

typedef struct
{
    char name[200];
    int type;
    cl_command_queue_info flag;
    char help[1000];
} infos;

extern const infos infosT[20];

const char *ltpv_commands_type_string(cl_command_type type);
