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
#ifndef PROFILER_GENERIC
	#define PROFILER_GENERIC
	
	struct profiler_t_generic_chrono {
		long time;
		profiler_t_generic_chrono *next;
	};
	typedef struct profiler_t_generic_chrono profiler_t_generic_chrono_list;

	struct profiler_t_generic_task {
		long idTask;
		long idStream;
		profiler_t_generic_task *next;
	};
	typedef struct profiler_t_generic_task profiler_t_generic_task;


	void profiler_generic_destroy_tasks_list();

	long profiler_generic_addDevice(
		const void *name,
		void *details = NULL
	);

	long profiler_generic_addStream(
		const void *name,
		const int idDevice = -1
	);

	long profiler_generic_addTask(
		const void *name,
		const int idStream = -1
	);

	long profiler_generic_taskInstanceStart();

	void profiler_generic_addTaskInstance(
		const int idTask,
		const int idChrono = 0,
		const int idStream = -1
	);
#endif
