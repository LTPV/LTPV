#define DEBUG(status)\
	do {\
		if(status != CL_SUCCESS) {\
			cout << RED << __FILE__ << ":" << __LINE__ << " ERROR: ";\
			switch(status) {\
				case CL_INVALID_PROGRAM					: cout << "CL_INVALID_PROGRAM"					; break;\
				case CL_INVALID_PROGRAM_EXECUTABLE		: cout << "CL_INVALID_PROGRAM_EXECUTABLE"		; break;\
				case CL_INVALID_KERNEL					: cout << "CL_INVALID_KERNEL" 					; break;\
				case CL_INVALID_KERNEL_NAME				: cout << "CL_INVALID_KERNEL_NAME"				; break;\
				case CL_INVALID_KERNEL_DEFINITION		: cout << "CL_INVALID_KERNEL_DEFINITION"		; break;\
				case CL_INVALID_VALUE					: cout << "CL_INVALID_VALUE"					; break;\
				case CL_INVALID_ARG_INDEX				: cout << "CL_INVALID_ARG_INDEX" 				; break;\
				case CL_INVALID_ARG_VALUE				: cout << "CL_INVALID_ARG_VALUE" 				; break;\
				case CL_INVALID_SAMPLER					: cout << "CL_INVALID_SAMPLER" 					; break;\
				case CL_INVALID_ARG_SIZE				: cout << "CL_INVALID_ARG_SIZE" 				; break;\
				case CL_INVALID_COMMAND_QUEUE			: cout << "CL_INVALID_COMMAND_QUEUE" 			; break;\
				case CL_INVALID_CONTEXT					: cout << "CL_INVALID_CONTEXT" 					; break;\
				case CL_INVALID_KERNEL_ARGS				: cout << "CL_INVALID_KERNEL_ARGS" 				; break;\
				case CL_INVALID_WORK_DIMENSION			: cout << "CL_INVALID_WORK_DIMENSION" 			; break;\
				case CL_INVALID_WORK_GROUP_SIZE			: cout << "CL_INVALID_WORK_GROUP_SIZE" 			; break;\
				case CL_INVALID_WORK_ITEM_SIZE			: cout << "CL_INVALID_WORK_ITEM_SIZE" 			; break;\
				case CL_INVALID_GLOBAL_OFFSET			: cout << "CL_INVALID_GLOBAL_OFFSET" 			; break;\
				case CL_INVALID_EVENT_WAIT_LIST			: cout << "CL_INVALID_EVENT_WAIT_LIST" 			; break;\
				case CL_OUT_OF_RESOURCES				: cout << "CL_OUT_OF_RESOURCES" 				; break;\
				case CL_MEM_OBJECT_ALLOCATION_FAILURE	: cout << "CL_MEM_OBJECT_ALLOCATION_FAILURE" 	; break;\
				case CL_INVALID_MEM_OBJECT				: cout << "CL_INVALID_MEM_OBJECT" 				; break;\
				case CL_MEM_COPY_OVERLAP				: cout << "CL_MEM_COPY_OVERLAP" 				; break;\
				case CL_OUT_OF_HOST_MEMORY				: cout << "CL_OUT_OF_HOST_MEMORY" 				; break;\
				case CL_PROFILING_INFO_NOT_AVAILABLE	: cout << "CL_PROFILING_INFO_NOT_AVAILABLE"		; break;\
				case CL_INVALID_EVENT					: cout << "CL_INVALID_EVENT"					; break;\
				default									: cout << "CL_UNKNOWN" 							; break;\
			}\
			cout << ENDS << endl;\
			return;\
		}\
	} while(0)
