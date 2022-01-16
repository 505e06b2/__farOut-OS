//kernel
#include "fs.h"
#include "task.h"

//stdlib
#include "sys.h"

int __far segmentmap(uint8_t drive_id, const char *filename, uint16_t segment) {
	return mapFileToSegment(drive_id, filename, segment);
}

int __far exec(uint8_t drive_id, const char *filename, const char *argv) {
	return executeCOM(drive_id, filename, argv);
}
