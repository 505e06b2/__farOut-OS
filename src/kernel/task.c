#include "task.h"

task_t *task_create(const drive_info_t *drive_info, const uint16_t requested_segment, const char *filename, task_t *buffer) {
	file_info_t file_info;

	if(findFileInfo(drive_info, filename, &file_info) == NULL) {
		printf("Could not find \"%s\" on disk!\r\n", filename);
		halt();
	}

	copyFileContents(drive_info, &file_info, requested_segment);

	static size_t id = -1; //-1 will overflow into 0
	buffer->id = ++id;
	buffer->segment_location = requested_segment;

	return buffer;
}

void task_run(task_t *task) {
	void __far (*program)() = SEGMENT_TO_FAR_POINTER(task->segment_location);
	program();
}
