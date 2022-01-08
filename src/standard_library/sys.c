//kernel
#include "fs.h"

//stdlib
#include "sys.h"

int __far segmentmap(uint8_t drive_id, const char *filename, uint16_t segment) {
	return mapFileToSegment(drive_id, filename, segment);
}
