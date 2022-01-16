#include "task.h"

task_t *task_create(
	const drive_info_t *drive_info,
	const uint16_t requested_segment,
	const char *filename,
	const char *argv,

	task_t *ret
) {
	file_info_t file_info;

	if(findFileInfo(drive_info, filename, &file_info) == NULL) return NULL;

	copyFileContents(drive_info, &file_info, requested_segment);

	static size_t id = -1; //-1 will overflow into 0 on addition
	ret->id = ++id;
	ret->segment_location = requested_segment;

	//"in-segment" calculations
	void *heap_location = 0;
	heap_location += file_info.size;
	heap_location += (size_t)heap_location % 2; //align heap to 2 bytes
	ret->heap_location = heap_location;

	volatile uint8_t __far *requested_segment_heap = (volatile uint8_t __far *)SEGMENT_TO_FAR_POINTER(requested_segment);
	requested_segment_heap += (size_t)heap_location;
	if(argv) copyStringMemory((volatile char __far *)requested_segment_heap, argv);

	//add stack loc to task_t at some point?
	volatile uint8_t __far *requested_segment_stack = (volatile uint8_t __far *)SEGMENT_TO_FAR_POINTER(requested_segment) + 0xffff + 1; //+1 so the end lines up with 0x*ffff
	requested_segment_stack -= sizeof(program_args_t);
	{
		program_args_t arguments;
		if(argv) {
			arguments.argv = heap_location;
		} else {
			arguments.argv = NULL;
		}
		arguments.boot_drive_id = drive_info->id;
		copyMemory(requested_segment_stack, &arguments, sizeof(program_args_t));
	}

	return ret;
}

void task_run(task_t *task) {
	void __far (*program)() = (volatile void __far *)SEGMENT_TO_FAR_POINTER(task->segment_location);
	program();
}

int __far executeCOM(uint8_t drive_id, const char *filename, const char *argv) {
	drive_info_t drive_info;
	task_t current_task;

	if(findDriveInfo(drive_id, &drive_info) == NULL) return 1;

	register uint16_t stack_segment asm("ss");
	uint16_t new_segment = stack_segment-0x1000;
	if(new_segment <= 0x1000) {
		printChar('H'); //Halt
		printChar(' ');
		printChar('T'); //Task
		printChar('M'); //Memory
		halt();
	}

	if(task_create(&drive_info, new_segment, filename, argv, &current_task) == NULL) return 1;
	task_run(&current_task);
	return 0;
}
