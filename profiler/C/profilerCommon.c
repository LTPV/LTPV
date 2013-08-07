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
#include "profilerCommon.h"

profiler_t_device *profiler_devicesInstanciated = NULL;
profiler_t_task *profiler_tasksInstanciated = NULL;
profiler_t_end_functions *profiler_end_functions = NULL;
long profiler_t0;

void profiler_start() {
	struct timeval t;
	gettimeofday(&t, NULL);
	profiler_t0 = t.tv_sec*1000000+t.tv_usec;
	return;
}

void profiler_addDevice(
	int idDevice,
	const void *nameDevice,
	void *detailsDevice,
	long timeOffset
) {
	if(PROFILING==ON) {
		profiler_t_device *newDevice = (profiler_t_device*) malloc(sizeof(profiler_t_device));
		newDevice->id = idDevice;
		strcpy(newDevice->name, (const char*)nameDevice);
		newDevice->details = (char*)detailsDevice; // Will be freed later
		newDevice->timeOffset = timeOffset;
		newDevice->firstStream=NULL;
		newDevice->next=NULL;
		profiler_t_device **devicePtr = &profiler_devicesInstanciated;
		while(*devicePtr != NULL) devicePtr = &((*devicePtr)->next);
		*devicePtr = newDevice;
	}
}

void profiler_addStream(
	int idStream,
	int idDevice,
	const void *name
) {
	if(PROFILING==ON) {
		profiler_t_stream *newStream = (profiler_t_stream*) malloc(sizeof(profiler_t_stream));
		strcpy(newStream->name, (const char*)name);
		newStream->id = idStream;
		newStream->firstTaskInstance = NULL;
		newStream->next      = NULL;
		
		profiler_t_device *device = profiler_devicesInstanciated;
		while(device->id != idDevice) device = device->next;

		profiler_t_stream **streamPtr = &device->firstStream;
		while(*streamPtr!=NULL) streamPtr = &((*streamPtr)->next);
		*streamPtr = newStream;
	}
}

void profiler_addTask(
	int idTask,
	const void *nameTask
) {
	if(PROFILING==ON) {
		profiler_t_task *newTask = (profiler_t_task*) malloc(sizeof(profiler_t_task));
		newTask->id = idTask;
		strcpy(newTask->name, (const char*)nameTask);
		newTask->next=NULL;
		
		unsigned int i = 0;
		profiler_t_task **ptrTask = &profiler_tasksInstanciated;
		while(*ptrTask != NULL) {
			ptrTask = &((*ptrTask)->next);
			i++;
		}
		if(idTask==-1) newTask->id = i;
		*ptrTask = newTask;
	}
}

void profiler_addTaskInstance(
	int idTask,
	const char* name,
	char* details,
	int idStream,
	long queued,
	long submitted,
	long start,
	long end,
	long size,
	long bandwidth
) {
	profiler_t_taskInstance *taskInstance = (profiler_t_taskInstance*) malloc(sizeof(profiler_t_taskInstance));
	taskInstance->idTask    = idTask;
	strcpy(taskInstance->name, name);
	taskInstance->details   = details;
	taskInstance->queued    = queued;
	taskInstance->submitted = submitted;
	taskInstance->start     = start;
	taskInstance->end       = end;
	taskInstance->size      = size;
	taskInstance->bandwidth = bandwidth;
	taskInstance->next      = NULL;

	// Find the stream
	profiler_t_stream *stream;
	profiler_t_device *idDevice = profiler_devicesInstanciated;
	int stopSearch = 0;
	while(idDevice != NULL && stopSearch == 0) {
		stream = idDevice->firstStream;
		while(stream !=NULL && stopSearch == 0) {
			if(stream->id == idStream) stopSearch=1;
			else stream = stream -> next;
		}
		idDevice = idDevice->next;
	}
	
	profiler_t_taskInstance **ptrTaskInstance = &(stream->firstTaskInstance);
	while(*ptrTaskInstance != NULL) {
		ptrTaskInstance = &((*ptrTaskInstance)->next);
	}
	*ptrTaskInstance = taskInstance;
}

void profiler_stopAndRecord(
	const void *filename
) {
	if(PROFILING==ON) {
		// call callback functions
		profiler_t_end_functions *functionS = profiler_end_functions, *functionSN;
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
		profiler_t_task *task = profiler_tasksInstanciated;
		while(task != NULL) {
			fprintf(f, "\t<task>\n\t\t<id>%d</id>\n\t\t<name>%s</name>\n\t</task>\n", task->id, task->name);
			task = task->next;
		}
		/* List devices */\
		fprintf(f, "\n\t<!-- List devices -->\n");
		profiler_t_device *device = profiler_devicesInstanciated;
		while(device != NULL) {
			fprintf(f, "\t<device><!-- device->id = %d -->\n\t\t<name>%s</name>\n\t\t<details>", device->id, device->name);
			if(device->details != NULL) {
				fprintf(f, "\n%s", device->details);
				free(device->details);
			}
			fprintf(f, "\t\t</details>\n");
			profiler_t_stream *stream = device->firstStream;
			while(stream!=NULL) {
				fprintf(f, "\t\t<stream>\n\t\t\t<name>%s</name>\n", stream->name);
				profiler_t_taskInstance *taskInstance = stream->firstTaskInstance;
				while(taskInstance!=NULL) {
					char taskName[500]="";
					task = profiler_tasksInstanciated;
					while(task != NULL) {
						if(task->id == taskInstance->idTask) {
							strcpy(taskName, task->name);
							task = NULL;
						}
						else {
							task = task->next;
						}
					}
					if(taskInstance->start     + device->timeOffset-profiler_t0 >= 0) { // if start < 0, it means that the task was queued before the start call
						fprintf(
							f,
							"\t\t\t<taskInstance>\n\t\t\t\t<idTask>%d</idTask><!-- task.name = %s -->\n\t\t\t\t<name>%s</name>\n\t\t\t\t<queued>%ld</queued>\n\t\t\t\t<submitted>%ld</submitted>\n\t\t\t\t<started>%ld</started>\n\t\t\t\t<ended>%ld</ended>%s\n",
							taskInstance->idTask,
							taskName,
							(taskInstance->name[0] == '\0') ? taskName : taskInstance->name,
							taskInstance->queued    + device->timeOffset-profiler_t0,
							taskInstance->submitted + device->timeOffset-profiler_t0,
							taskInstance->start     + device->timeOffset-profiler_t0,
							taskInstance->end       + device->timeOffset-profiler_t0,
							(taskInstance->details != NULL) ? taskInstance->details : ""
						);
						if(taskInstance->size>0) {
							fprintf(f, "\t\t\t\t<size>%ld</size>\n", taskInstance->size);
						}
						if(taskInstance->size>0) {
							fprintf(f, "\t\t\t\t<bandwidth>%ld</bandwidth>\n", taskInstance->bandwidth);
						}
						fprintf(f, "\t\t\t</taskInstance>\n");
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
			profiler_t_task *task = profiler_tasksInstanciated, *taskN;
			while(task != NULL) {
				taskN = task->next;
				free(task);
				task = taskN;
			}
			profiler_t_device *device = profiler_devicesInstanciated, *deviceN;
			while(device != NULL) {
				profiler_t_stream *stream = device->firstStream, *streamN;
				while(stream!=NULL) {
					profiler_t_taskInstance *taskInstance = stream->firstTaskInstance, *taskInstanceN;
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
}