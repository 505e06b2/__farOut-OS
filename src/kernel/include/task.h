#ifndef _TASK_H
#define _TASK_H

#include "fs.h"
#include "stdint.h"

#include "start.h"

typedef struct task_s {
	size_t id;
	uint16_t segment_location;
} task_t; //add more here as needed

task_t *task_create(
	const drive_info_t *,
	const uint16_t,
	const char *,
	const start_data_t *,

	task_t *
);
void task_run(task_t *);

#endif
