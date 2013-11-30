/*
 * (C) Copyright 2013 - Thales SA (author: Simon DENEL - Thales Research & Technology)
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
#include "libLTPV_common.h"

#include "ltpv_tools.c"

ltpv_t_device        *ltpv_devicesInstanciated = NULL;
ltpv_t_task          *ltpv_tasksInstanciated   = NULL;
ltpv_t_end_functions *ltpv_end_functions       = NULL;
long ltpv_t0;

void ltpv_start() {
	struct timeval t;
	gettimeofday(&t, NULL);
	ltpv_t0 = t.tv_sec*1000000+t.tv_usec;
	return;
}

void ltpv_addDevice(
	int idDevice,
	const void *nameDevice,
	void *detailsDevice,
	long timeOffset
) {

	ltpv_t_device *newDevice = (ltpv_t_device*) malloc(sizeof(ltpv_t_device));
	newDevice->id = idDevice;
	strcpy(newDevice->name, (const char*)nameDevice);
	newDevice->details = (char*)detailsDevice; // Will be freed later
	newDevice->timeOffset = timeOffset;
	newDevice->firstStream=NULL;
	newDevice->next=NULL;
	ltpv_t_device **devicePtr = &ltpv_devicesInstanciated;
	while(*devicePtr != NULL) devicePtr = &((*devicePtr)->next);
	*devicePtr = newDevice;
}

void ltpv_addStream(
	int idStream,
	int idDevice,
	const void *name
) {
	ltpv_t_stream *newStream = (ltpv_t_stream*) malloc(sizeof(ltpv_t_stream));
	strcpy(newStream->name, (const char*)name);
	newStream->id = idStream;
	newStream->firstTaskInstance = NULL;
	newStream->next      = NULL;
	
	ltpv_t_device *device = ltpv_devicesInstanciated;
	while(device->id != idDevice) device = device->next;

	ltpv_t_stream **streamPtr = &device->firstStream;
	while(*streamPtr!=NULL) streamPtr = &((*streamPtr)->next);
	*streamPtr = newStream;
}

void ltpv_addTask(
	unsigned long idTask,
	const void *nameTask
) {
	ltpv_t_task *newTask = (ltpv_t_task*) malloc(sizeof(ltpv_t_task));
	newTask->id = idTask;
	strcpy(newTask->name, (const char*)nameTask);
	newTask->next=NULL;
	
	unsigned int i = 0;
	ltpv_t_task **ptrTask = &ltpv_tasksInstanciated;
	while(*ptrTask != NULL) {
		ptrTask = &((*ptrTask)->next);
		i++;
	}
	if(idTask==-1) newTask->id = i;
	*ptrTask = newTask;
}

void ltpv_addTaskInstance(
	unsigned long idTask,
	const char* name,
	char* details,
	int idStream,
	long start,
	long end,
	long ocl_queued,
	long ocl_submit,
	long ocl_size,
	long ocl_bandwidth
) {
	ltpv_t_taskInstance *taskInstance = (ltpv_t_taskInstance*) malloc(sizeof(ltpv_t_taskInstance));
	taskInstance->idTask    = idTask;
	strcpy(taskInstance->name, name);
	taskInstance->details   = details;
	//printf("[%ld %ld]\n", ocl_queued, start);
	taskInstance->start         = start;
	taskInstance->end           = end;
	taskInstance->ocl_queued    = ocl_queued;
	taskInstance->ocl_submit    = ocl_submit;
	taskInstance->ocl_size      = ocl_size;
	taskInstance->ocl_bandwidth = ocl_bandwidth;
	taskInstance->next          = NULL;

	// Find the stream
	ltpv_t_stream *stream;
	ltpv_t_device *idDevice = ltpv_devicesInstanciated;
	int stopSearch = 0;
	while(idDevice != NULL && stopSearch == 0) {
		stream = idDevice->firstStream;
		while(stream !=NULL && stopSearch == 0) {
			if(stream->id == idStream) stopSearch=1;
			else stream = stream -> next;
		}
		idDevice = idDevice->next;
	}
	if(LTPV_DEBUG && stopSearch==0) printf("Error %s:%d: stopSearch=0\n", __FILE__, __LINE__);
	
	ltpv_t_taskInstance **ptrTaskInstance = &(stream->firstTaskInstance);
	while(*ptrTaskInstance != NULL) {
		ptrTaskInstance = &((*ptrTaskInstance)->next);
	}
	*ptrTaskInstance = taskInstance;
}

void ltpv_stopAndRecord(
	const void *filename
) {
	// call callback functions
	ltpv_t_end_functions *functionS = ltpv_end_functions, *functionSN;
	while(functionS!=NULL) {
		void (*function)() = functionS->function;
		(*function)();
		functionSN = functionS->next;
		free(functionS);
		functionS = functionSN;
	}
	FILE * f = fopen((const char*)filename, "w");
	fprintf(f, "<profiling>\n\t<head>\n\t\t<unit>μs</unit>\n");
	{ // Date
		char arrDayNames[7][15] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
		char arrMonthNames[12][15]={"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
		time_t t = time(NULL);
		struct tm tm = *localtime(&t);
		fprintf(f, "\t<date>%s, %s %2d&lt;sup&gt;th&lt;/sup&gt; %d at &lt;b&gt;%2d:%2d:%2d&lt;/b&gt;</date>\n", arrDayNames[tm.tm_wday], arrMonthNames[tm.tm_mon], tm.tm_mday, tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec);
	}
	fprintf(f, "\t</head>\n");
	/* List tasks */
	fprintf(f, "\n\t<!-- List tasks -->\n");
	ltpv_t_task *task = ltpv_tasksInstanciated;
	while(task != NULL) {
		fprintf(f, "\t<task>\n\t\t<id>%ld</id>\n\t\t<name>%s</name>\n\t</task>\n", task->id, task->name);
		task = task->next;
	}
	/* List devices */\
	fprintf(f, "\n\t<!-- List devices -->\n");
	ltpv_t_device *device = ltpv_devicesInstanciated;
	while(device != NULL) {
		fprintf(f, "\t<device><!-- device->id = %d -->\n\t\t<name>%s</name>\n\t\t<details>", device->id, device->name);
		if(device->details != NULL) {
			fprintf(f, "\n%s", device->details);
			free(device->details);
		}
		fprintf(f, "</details>\n");
		ltpv_t_stream *stream = device->firstStream;
		while(stream!=NULL) {
			fprintf(f, "\t\t<stream>\n\t\t\t<name>%s</name>\n", stream->name);
			ltpv_t_taskInstance *taskInstance = stream->firstTaskInstance;
			while(taskInstance!=NULL) {
				char taskName[500]="";
				task = ltpv_tasksInstanciated;
				while(task != NULL) {
					if(task->id == taskInstance->idTask) {
						strcpy(taskName, task->name);
						task = NULL;
					}
					else {
						task = task->next;
					}
				}
				if(taskInstance->start     + device->timeOffset-ltpv_t0 >= 0) { // if start < 0, it means that the task was queued before the start call
					fprintf(
						f,
						"\t\t\t<taskInstance>\n\t\t\t\t<idTask>%ld</idTask><!-- task.name = %s -->\n\t\t\t\t<name>%s</name>\n\t\t\t\t<start>%ld</start>\n\t\t\t\t<end>%ld</end>%s\n",
						taskInstance->idTask,
						taskName,
						(taskInstance->name[0] == '\0') ? taskName : taskInstance->name,
						taskInstance->start     + device->timeOffset-ltpv_t0,
						taskInstance->end       + device->timeOffset-ltpv_t0,
						(taskInstance->details != NULL) ? taskInstance->details : ""
					);
/*
						taskInstance->ocl_queued    + device->timeOffset-ltpv_t0,
						taskInstance->ocl_submit    + device->timeOffset-ltpv_t0,
						*/
					if(taskInstance->ocl_queued>=0) {
						fprintf(f, "\t\t\t\t<ocl_queued>%ld</ocl_queued>\n", taskInstance->ocl_queued + device->timeOffset - ltpv_t0);
					}
					if(taskInstance->ocl_submit>=0) {
						fprintf(f, "\t\t\t\t<ocl_submit>%ld</ocl_submit>\n", taskInstance->ocl_submit + device->timeOffset - ltpv_t0);
					}
					if(taskInstance->ocl_size>0) {
						fprintf(f, "\t\t\t\t<ocl_size>%ld</ocl_size>\n", taskInstance->ocl_size);
					}
					if(taskInstance->ocl_size>0) {
						fprintf(f, "\t\t\t\t<ocl_bandwidth>%ld</ocl_bandwidth>\n", taskInstance->ocl_bandwidth);
					}
					fprintf(f, "\t\t\t</taskInstance>\n");
				}
				else if(LTPV_DEBUG) {
					printf("LTPV %s:%d: a taskInstance was seen as started before the origin and was therefore not written back: was it voluntary?\n", __FILE__, __LINE__);
				}
				taskInstance = taskInstance->next;
			}
			fprintf(f, "\t\t</stream>\n");
			stream = stream->next;
		}
		fprintf(f, "\t</device>\n");
		device = device->next;
	}
	fprintf(f, "</profiling>\n");
	fclose(f);
	
	// Let's clean our mess
	{
		ltpv_t_task *task = ltpv_tasksInstanciated, *taskN;
		while(task != NULL) {
			taskN = task->next;
			free(task);
			task = taskN;
		}
		ltpv_t_device *device = ltpv_devicesInstanciated, *deviceN;
		while(device != NULL) {
			ltpv_t_stream *stream = device->firstStream, *streamN;
			while(stream!=NULL) {
				ltpv_t_taskInstance *taskInstance = stream->firstTaskInstance, *taskInstanceN;
				while(taskInstance != NULL) {
					taskInstanceN = taskInstance->next;
						free(taskInstance->details);
					free(taskInstance);
					taskInstance = taskInstanceN;
				}
				streamN = stream->next;
				free(stream);
				stream = streamN;
			}
			deviceN = device->next;
			free(device);
			device = deviceN;
		}
	}
}
