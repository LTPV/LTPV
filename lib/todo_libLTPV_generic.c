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
#include "profilerGeneric.h"

extern profiler_t_end_functions *profiler_end_functions;

profiler_t_generic_task *profiler_generic_tasks_list = NULL;
profiler_t_generic_chrono *profiler_generic_chrono_list = NULL;



long profiler_generic_posIdDevice = 0;
long profiler_generic_posIdStream = 0;
long profiler_generic_posIdTask   = 0;

long profiler_generic_initialize = 0;

void profiler_generic_destroy_tasks_list() {
	profiler_t_generic_task *task = profiler_generic_tasks_list, *taskN;
	while(task!=NULL) {
		taskN = task->next;
		free(task);
		task = taskN;
	}
}

long profiler_generic_addDevice(
	const void *name,
	void *details
) {
	if(profiler_generic_initialize==0) {
		profiler_generic_initialize = 1;
		profiler_t_end_functions *newEndFunction = (profiler_t_end_functions*)malloc(sizeof(profiler_t_end_functions));
		newEndFunction->function = profiler_generic_destroy_tasks_list;
		newEndFunction->next=NULL;
		profiler_t_end_functions ** ptrEndFunction = &profiler_end_functions;
		while(*ptrEndFunction!=NULL) {
			ptrEndFunction = &((*ptrEndFunction)->next);
		}
		*ptrEndFunction = newEndFunction;
	}
	long idDevice = profiler_generic_posIdDevice;
	profiler_addDevice(idDevice, name, details);
	profiler_generic_posIdDevice++;
	return idDevice;
}

long profiler_generic_addStream(
	const void *name,
	const int idDevice
) {
	// idDevice == -1 => Taking the last one declared
	long idStream = profiler_generic_posIdStream;
	profiler_addStream(idStream, idDevice==-1 ? (profiler_generic_posIdDevice-1) : idDevice, name);
	profiler_generic_posIdStream++;
	return idStream;
}

long profiler_generic_addTask(
	const void *name,
	const int idStream
) {
	// idStream == 1- => Taking the last one declared
	long idTask = profiler_generic_posIdTask;
	profiler_t_generic_task *task = (profiler_t_generic_task*)malloc(sizeof(profiler_t_generic_task));
	task->idTask = idTask;
	task->idStream = profiler_generic_posIdStream-1;
	task->next=NULL;
	profiler_t_generic_task **tasksList = &profiler_generic_tasks_list;
	while(*tasksList!=NULL) {
		tasksList = &((*tasksList)->next);
	}
	*tasksList = task;
	profiler_addTask(idTask, name);
	profiler_generic_posIdTask++;
	return idTask;
}

long profiler_generic_taskInstanceStart() {
	profiler_t_generic_chrono *chrono = (profiler_t_generic_chrono*)malloc(sizeof(profiler_t_generic_chrono));

	chrono->next = NULL;
	profiler_t_generic_chrono **chronoPtr = &profiler_generic_chrono_list;
	while(*chronoPtr != NULL) {
		chronoPtr = &((*chronoPtr)->next);
	}
	*chronoPtr = chrono;
	
	struct timeval start;
	gettimeofday(&start, NULL);
	long t = start.tv_sec*1000000+start.tv_usec;
	chrono->time = t;
	return ((long)chrono);
}

void profiler_generic_addTaskInstance(
	const int idTask,
	const int idChrono,
	const int idStream
) {
	// idChrono == 0 => Taking the last chrono created
	// stream = -1 => the stream attached to task
	struct timeval stop;
	gettimeofday(&stop, NULL);
	long t = stop.tv_sec*1000000+stop.tv_usec;

	profiler_t_generic_chrono **chronoPtr = &profiler_generic_chrono_list, **chronoPtrP = &profiler_generic_chrono_list;
	while((*chronoPtr)->next != NULL && (long)(*chronoPtr) != idChrono) {
		chronoPtrP = chronoPtr;
		chronoPtr = &((*chronoPtr)->next);
	}
	if(idStream!=-1) {
		profiler_addTaskInstance(idTask, "", NULL, idStream, 0, 0, (*chronoPtr)->time, t);
	}
	else {
		profiler_t_generic_task *tasksList = profiler_generic_tasks_list;
		while(tasksList!=NULL && tasksList->idTask!=idTask) {
			tasksList = tasksList->next;
		}
		profiler_addTaskInstance(idTask, "", NULL, tasksList->idStream, (*chronoPtr)->time, (*chronoPtr)->time, (*chronoPtr)->time, t);
	}
	
	profiler_t_generic_chrono *ptr=(*chronoPtr)->next;
	free(*chronoPtr);
	*chronoPtrP = ptr;
	return;
}
