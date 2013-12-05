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

#include <stdarg.h>

#include "libLTPV.h"

#include "libLTPV_common.c"
#include "libLTPV_OpenCL.c"


static void wrap_init(void) __attribute__((constructor));
static void wrap_end (void) __attribute__((destructor ));


static void wrap_init(void)
{
        ltpv_start();
}

static void wrap_end(void)
{
        ltpv_stopAndRecord("profiling_date.xml");
}


struct ltpv_buffer_elem {
	char name[100];
	long start;
	long stop;
};
typedef struct ltpv_buffer_elem ltpv_buffer_elem;
//ltpv_buffer_elem ltpv_buffer[LTPV_NUMTHREADS][LTPV_SIZEBUFFER];
//unsigned int ltpv_buffer_pos[LTPV_NUMTHREADS] = {0};


struct ltpv_t_cpu_task {
	char name[100];
	struct ltpv_t_cpu_task *next;
};
typedef struct ltpv_t_cpu_task ltpv_t_cpu_task;
ltpv_t_cpu_task *ltpv_cpu_task = NULL;

struct ltpv_t_cpu_stream {
	long threadId;
	char name[100];
	struct ltpv_t_cpu_stream *next;
};
typedef struct ltpv_t_cpu_stream ltpv_t_cpu_stream;
ltpv_t_cpu_stream *ltpv_cpu_stream = NULL;


int isInitialized = 0;

//int fprintf(FILE * stream, const char * format, ... ) {
int puts(const char *str) {
	if(strlen(str) > 6) if(
		str[0] == '[' &&
		str[1] == 'L' &&
		str[2] == 'T' &&
		str[3] == 'P' &&
		str[4] == 'V' &&
		str[5] == ']'
	) {
		if(isInitialized==0) {
			ltpv_addDevice((long)&isInitialized, "CPU");
			// add quit function here for frees !
		}


		unsigned int       ltpv_numthreads;// ltpv_numthreads = va_arg(arg, unsigned int      );
		unsigned int       ltpv_sizebuffer;// ltpv_sizebuffer = va_arg(arg, unsigned int      );
		ltpv_buffer_elem * ltpv_buffer    ;// ltpv_buffer     = va_arg(arg, ltpv_buffer_elem *);
		unsigned int     * ltpv_buffer_pos;// ltpv_buffer_pos = va_arg(arg, unsigned int *    );
		
		//sprintf(s, "echo \"%s\" > hello", str);
		sscanf(str, "[LTPV] %d %d %d %d", (int*)&ltpv_numthreads,(int*)&ltpv_sizebuffer, (int*)&ltpv_buffer, (int*)&ltpv_buffer_pos);
		//char s[200];
		//sprintf(s, "echo \"%d %d %d %d\" > hello", ltpv_numthreads,ltpv_sizebuffer, ltpv_buffer, ltpv_buffer_pos);
		//system(s);
		
		
		
		//printf("GOTCHA! \"%s\" [ltpv_numthreads=%d][ltpv_sizebuffer=%d][ltpv_buffer=0x%lX][ltpv_buffer_pos=0x%lX]\n", format, ltpv_numthreads, ltpv_sizebuffer, (unsigned long)ltpv_buffer, (unsigned long)ltpv_buffer_pos);
		for(unsigned int i = 0; i < ltpv_numthreads; i++) {
			// Is the thread stream already existing? Of not, let's create it.
			ltpv_t_cpu_stream** ltpv_cpu_stream_check      = &ltpv_cpu_stream;
			ltpv_t_cpu_stream*  ltpv_cpu_stream_check_prev = NULL;
			unsigned int alreadyExisting = 0;
			while(*ltpv_cpu_stream_check != NULL) {
				if((*ltpv_cpu_stream_check)->threadId == i) {
					printf("Already existing stream\n");
					alreadyExisting = 1;
					break;
				}
				ltpv_cpu_stream_check_prev = *ltpv_cpu_stream_check;
				*ltpv_cpu_stream_check = (*ltpv_cpu_stream_check)->next;
			}
			if(alreadyExisting==0) {
				ltpv_t_cpu_stream *ltpv_cpu_stream_new;
				ltpv_cpu_stream_new              = (ltpv_t_cpu_stream*) malloc(sizeof(ltpv_t_cpu_stream));
				if(ltpv_cpu_stream_check_prev!=NULL) ltpv_cpu_stream_check_prev->next = ltpv_cpu_stream_new;
				*ltpv_cpu_stream_check = ltpv_cpu_stream_new;
				
				char streamName[10] = "";
				sprintf(streamName, "thread %d", i);
				ltpv_addStream((long)*ltpv_cpu_stream_check, (long)&isInitialized, streamName);
			}
		
			for(unsigned int j = 0; j < ltpv_buffer_pos[i]; j++) {
				//printf("thread=%d j=%d [start=%ld][stop=%ld][name=%s]\n", i, j, ltpv_buffer[i*ltpv_sizebuffer+j].start, ltpv_buffer[i*ltpv_sizebuffer+j].stop, ltpv_buffer[i*ltpv_sizebuffer+j].name);
				ltpv_t_cpu_task*  ltpv_cpu_task_check      = ltpv_cpu_task;
				ltpv_t_cpu_task*  ltpv_cpu_task_check_prev = NULL;
				unsigned int k = 0;
				unsigned int alreadyExisting = 0;
				//printf("\n\nNew query: %s, ltpv_cpu_task = %ld\n", ltpv_buffer[i*ltpv_sizebuffer+j].name, ltpv_cpu_task);
				while(ltpv_cpu_task_check != NULL) {
					//printf("\t\tComparing %s with %s having next = %ld\n", ltpv_buffer[i*ltpv_sizebuffer+j].name, (ltpv_cpu_task_check)->name, (ltpv_cpu_task_check)->next);
					if(strcmp((ltpv_cpu_task_check)->name, ltpv_buffer[i*ltpv_sizebuffer+j].name)==0) {
						//printf("\tAlready existing task\n");
						alreadyExisting = 1;
						break;
					}
					ltpv_cpu_task_check_prev =  ltpv_cpu_task_check;
					ltpv_cpu_task_check      = (ltpv_cpu_task_check)->next;
				}
				if(alreadyExisting==0) {
					//printf("Creating task %s with ltpv_cpu_task_check_prev=%ld\n", ltpv_buffer[i*ltpv_sizebuffer+j].name, (long)ltpv_cpu_task_check_prev);
					//ltpv_cpu_task_check
					ltpv_t_cpu_task *ltpv_cpu_task_new;
					ltpv_cpu_task_new = (ltpv_t_cpu_task*)malloc(sizeof(ltpv_t_cpu_task));
					if(ltpv_cpu_task_check_prev!=NULL)
						ltpv_cpu_task_check_prev->next = ltpv_cpu_task_new;
					ltpv_cpu_task_check            = ltpv_cpu_task_new;
					if(ltpv_cpu_task==NULL)
						ltpv_cpu_task = ltpv_cpu_task_new;
					
					strcpy(ltpv_cpu_task_new->name, ltpv_buffer[i*ltpv_sizebuffer+j].name);
					ltpv_cpu_task_new->next = NULL;
					ltpv_addTask(((long)ltpv_cpu_task_new)+512*1024*1024, ltpv_buffer[i*ltpv_sizebuffer+j].name);
				}
				ltpv_addTaskInstance((long)(ltpv_cpu_task_check)+512*1024*1024, (char*)"", NULL, (long)*ltpv_cpu_stream_check, ltpv_buffer[i*ltpv_sizebuffer+j].start, ltpv_buffer[i*ltpv_sizebuffer+j].stop);
				//printf("Taken stream: %lx\n", (long)ltpv_cpu_stream_check);
			}
		}
		return 0;
	}
	else {
		return ltpv_call_original(puts)(str);
	}
}