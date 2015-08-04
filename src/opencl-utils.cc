#include "opencl-utils.hh"

const char *ltpv_commands_type_string(cl_command_type type)
{
    switch (type)
    {
        case CL_COMMAND_NDRANGE_KERNEL:                 return "Ndrange kernel";                     //CL_COMMAND_NDRANGE_KERNEL               
        case CL_COMMAND_NATIVE_KERNEL:                  return "Native kernel";                      //CL_COMMAND_NATIVE_KERNEL
        case CL_COMMAND_READ_BUFFER:                    return "Read buffer";                        //CL_COMMAND_READ_BUFFER
        case CL_COMMAND_WRITE_BUFFER:                   return "Write buffer";                       //CL_COMMAND_WRITE_BUFFER
        case CL_COMMAND_COPY_BUFFER:                    return "Copy buffer";                        //CL_COMMAND_COPY_BUFFER
        case CL_COMMAND_READ_IMAGE:                     return "Read image";                         //CL_COMMAND_READ_IMAGE
        case CL_COMMAND_WRITE_IMAGE:                    return "Write image";                        //CL_COMMAND_WRITE_IMAGE
        case CL_COMMAND_COPY_IMAGE:                     return "Copy image";                         //CL_COMMAND_COPY_IMAGE
        case CL_COMMAND_COPY_IMAGE_TO_BUFFER:           return "Copy image to buffer";               //CL_COMMAND_COPY_IMAGE_TO_BUFFER
        case CL_COMMAND_COPY_BUFFER_TO_IMAGE:           return "Copy buffer to image";               //CL_COMMAND_COPY_BUFFER_TO_IMAGE
        case CL_COMMAND_MAP_BUFFER:                     return "Map buffer";                         //CL_COMMAND_MAP_BUFFER
        case CL_COMMAND_MAP_IMAGE:                      return "Map image";                          //CL_COMMAND_MAP_IMAGE
        case CL_COMMAND_UNMAP_MEM_OBJECT:               return "Unmap mem object";                   //CL_COMMAND_UNMAP_MEM_OBJECT
        case CL_COMMAND_MARKER:                         return "Marker";                             //CL_COMMAND_MARKER
        case CL_COMMAND_ACQUIRE_GL_OBJECTS:             return "Acquire gl objects";                 //CL_COMMAND_ACQUIRE_GL_OBJECTS
        case CL_COMMAND_RELEASE_GL_OBJECTS:             return "Release gl objects";                 //CL_COMMAND_RELEASE_GL_OBJECTS
        case CL_COMMAND_READ_BUFFER_RECT:               return "Read buffer rect";                   //CL_COMMAND_READ_BUFFER_RECT
        case CL_COMMAND_WRITE_BUFFER_RECT:              return "Write buffer rect";                  //CL_COMMAND_WRITE_BUFFER_RECT
        case CL_COMMAND_COPY_BUFFER_RECT:               return "Copy buffer rect";                   //CL_COMMAND_COPY_BUFFER_RECT
        case CL_COMMAND_USER:                           return "User";                               //CL_COMMAND_USER
//        case CL_COMMAND_BARRIER:                        return "Barrier";                            //CL_COMMAND_BARRIER
//        case CL_COMMAND_MIGRATE_MEM_OBJECTS:            return "Migrate mem objects";                //CL_COMMAND_MIGRATE_MEM_OBJECTS
//        case CL_COMMAND_FILL_BUFFER:                    return "Fill buffer";                        //CL_COMMAND_FILL_BUFFER
//        case CL_COMMAND_FILL_IMAGE:                     return "Fill image";                         //CL_COMMAND_FILL_IMAGE
//        case CL_COMMAND_SVM_FREE:                       return "Svm free";                           //CL_COMMAND_SVM_FREE
//        case CL_COMMAND_SVM_MEMCPY:                     return "Svm memcpy";                         //CL_COMMAND_SVM_MEMCPY
//        case CL_COMMAND_SVM_MEMFILL:                    return "Svm memfill";                        //CL_COMMAND_SVM_MEMFILL
//        case CL_COMMAND_SVM_MAP:                        return "Svm map";                            //CL_COMMAND_SVM_MAP
//        case CL_COMMAND_SVM_UNMAP:                      return "Svm unmap";                          //CL_COMMAND_SVM_UNMAP
//        case CL_COMMAND_GL_FENCE_SYNC_OBJECT_KHR:       return "Gl fence sync object khr";           //CL_COMMAND_GL_FENCE_SYNC_OBJECT_KHR
//        case CL_COMMAND_EGL_FENCE_SYNC_OBJECT_KHR:      return "Egl fence sync object khr";          //CL_COMMAND_EGL_FENCE_SYNC_OBJECT_KHR
//        case CL_COMMAND_ACQUIRE_D3D10 OBJECTS_KHR:      return "Acquire d3d10 objects khr";          //CL_COMMAND_ACQUIRE_D3D10 OBJECTS_KHR
//        case CL_COMMAND_RELEASE_D3D10 OBJECTS_KHR:      return "Release d3d10 objects khr";          //CL_COMMAND_RELEASE_D3D10 OBJECTS_KHR
//        case CL_COMMAND_ACQUIRE_D3D11 OBJECTS_KHR:      return "Acquire d3d11 objects khr";          //CL_COMMAND_ACQUIRE_D3D11 OBJECTS_KHR
//        case CL_COMMAND_RELEASE_D3D11 OBJECTS_KHR:      return "Release d3d11 objects khr";          //CL_COMMAND_RELEASE_D3D11 OBJECTS_KHR
//        case CL_COMMAND_ACQUIRE_DX9 MEDIA_SURFACES_KHR: return "Acquire dx9 media surfaces khr";     //CL_COMMAND_ACQUIRE_DX9 MEDIA_SURFACES_KHR
//        case CL_COMMAND_RELEASE_DX9 MEDIA_SURFACES_KHR: return "Release dx9 media surfaces khr"      //CL_COMMAND_RELEASE_DX9 MEDIA_SURFACES_KHR
    }
   return "None" ;
};

const infos infosT[20] =
{
    {
        "CL_DEVICE_VENDOR",
        LTPV_OPENCL_STRING,
        CL_DEVICE_VENDOR,
        "Vendor name string."
    },
    {
        "CL_DEVICE_TYPE",
        LTPV_OPENCL_DEVICE_TYPE,
        CL_DEVICE_TYPE,
        "The OpenCL device type. Currently supported values are one of or a combination of: CL_DEVICE_TYPE_CPU, CL_DEVICE_TYPE_GPU, CL_DEVICE_TYPE_ACCELERATOR, or CL_DEVICE_TYPE_DEFAULT."
    },
    {
        "CL_DEVICE_ADDRESS_BITS",
        LTPV_OPENCL_UINT,
        CL_DEVICE_ADDRESS_BITS,
        "The default compute device address space size specified as an unsigned integer value in bits. Currently supported values are 32 or 64 bits."
    },
    {
        "CL_DEVICE_EXTENSIONS",
        LTPV_OPENCL_STRING,
        CL_DEVICE_EXTENSIONS,
        "Returns a list of extension names"
    },
    {
        "CL_DEVICE_VERSION",
        LTPV_OPENCL_STRING,
        CL_DEVICE_VERSION,
        "OpenCL version string. Returns the OpenCL version supported by the device. This version string has the following format::\nOpenCL&lt;space&gt;&lt;major_version.minor_version&gt;&lt;space&gt;&lt;vendor-specific information&gt;\nThe major_version.minor_version value returned will be 1.0."
    },
    {
        "CL_DEVICE_GLOBAL_MEM_CACHE_SIZE",
        LTPV_OPENCL_ULONG,
        CL_DEVICE_GLOBAL_MEM_CACHE_SIZE,
        "Size of global memory cache in bytes."
    },
    {
        "CL_DEVICE_GLOBAL_ MEM_CACHELINE_SIZE",
        LTPV_OPENCL_UINT,
        CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE,
        " Size of global memory cache line in bytes."
    },
    {
        "CL_DEVICE_GLOBAL_MEM_SIZE", LTPV_OPENCL_ULONG, CL_DEVICE_GLOBAL_MEM_SIZE, "Size of global device memory in bytes."
    },
    {
        "CL_DEVICE_LOCAL_MEM_SIZE", LTPV_OPENCL_ULONG, CL_DEVICE_LOCAL_MEM_SIZE, "Size of local device memory in bytes."
    },
    {
        "CL_DEVICE_MAX_WORK_GROUP_SIZE", LTPV_OPENCL_SIZE_T, CL_DEVICE_MAX_WORK_GROUP_SIZE, "Maximum number of work-items in a work-group executing a kernel on a single compute unit, using the data parallel execution model. (Refer to clEnqueueNDRangeKernel). The minimum value is 1. "
    },
    {
        "CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS",
        LTPV_OPENCL_UINT,
        CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS,
        "Maximum dimensions that specify the global and local work-item IDs used by the data parallel execution model. (Refer to clEnqueueNDRangeKernel). The minimum value is 3 for devices that are not of type CL_DEVICE_TYPE_CUSTOM."
    },
    {
        "CL_DEVICE_MAX_WORK_ITEM_SIZES",
        LTPV_OPENCL_SIZE_T_ARRAY,
        CL_DEVICE_MAX_WORK_ITEM_SIZES,
        " Maximum number of work-items that can be specified in each dimension of the work-group to clEnqueueNDRangeKernel.Returns n size_t entries, where n is the value returned by the query for CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS."
    },
    {
        "CL_DEVICE_MEM_BASE_ADDR_ALIGN", LTPV_OPENCL_UINT, CL_DEVICE_MEM_BASE_ADDR_ALIGN, "The minimum value is the size (in bits) of the largest OpenCL built-in data type supported by the device (long16 in FULL profile, long16 or int16 in EMBEDDED profile) for devices that are not of type CL_DEVICE_TYPE_CUSTOM. "
    },
    {
        "CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR", LTPV_OPENCL_UINT, CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR, "Preferred native vector width size for built-in scalar types that can be put into vectors. The vector width is defined as the number of scalar elements that can be stored in the vector."
    },
    {
        "CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT", LTPV_OPENCL_UINT, CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT, "Preferred native vector width size for built-in scalar types that can be put into vectors. The vector width is defined as the number of scalar elements that can be stored in the vector."
    },
    {
        "CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT", LTPV_OPENCL_UINT, CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT, "Preferred native vector width size for built-in scalar types that can be put into vectors. The vector width is defined as the number of scalar elements that can be stored in the vector."
    },
    {
        "CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG", LTPV_OPENCL_UINT, CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG, "Preferred native vector width size for built-in scalar types that can be put into vectors. The vector width is defined as the number of scalar elements that can be stored in the vector."
    },
    {
        "CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT", LTPV_OPENCL_UINT, CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT, "Preferred native vector width size for built-in scalar types that can be put into vectors. The vector width is defined as the number of scalar elements that can be stored in the vector."
    },
    {
        "CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE", LTPV_OPENCL_UINT, CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE, "Preferred native vector width size for built-in scalar types that can be put into vectors. The vector width is defined as the number of scalar elements that can be stored in the vector."
    },
    {
        "CL_DEVICE_PREFERRED_VECTOR_WIDTH_HALF", LTPV_OPENCL_UINT, CL_DEVICE_PREFERRED_VECTOR_WIDTH_HALF, "Preferred native vector width size for built-in scalar types that can be put into vectors. The vector width is defined as the number of scalar elements that can be stored in the vector."
    }
    // {
    //     "",
    //     ,
    //     ,
    //     ""
    // },
};
