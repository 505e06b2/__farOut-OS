#include "task.h"

task_t *task_create(
	const drive_info_t *drive_info,
	const uint16_t requested_segment,
	const char *filename,
	const start_data_t *start_data,

	task_t *ret
) {
	file_info_t file_info;

	if(findFileInfo(drive_info, filename, &file_info) == NULL) {
		//printf("Could not find \"%s\" on disk!\r\n", filename);
		halt();
	}

	copyFileContents(drive_info, &file_info, requested_segment);

	static size_t id = -1; //-1 will overflow into 0 on addition
	ret->id = ++id;
	ret->segment_location = requested_segment;

	volatile uint8_t __far *requested_segment_stack = (volatile uint8_t __far *)SEGMENT_TO_FAR_POINTER(requested_segment) + 0xffff + 1; //+1 so the end lines up with 0x*ffff
	requested_segment_stack -= sizeof(start_data_t);
	copyMemory(requested_segment_stack, start_data, sizeof(start_data_t));

	return ret;
}

void task_run(task_t *task) {
	void __far (*program)() = (volatile void __far *)SEGMENT_TO_FAR_POINTER(task->segment_location);
	program();
}
