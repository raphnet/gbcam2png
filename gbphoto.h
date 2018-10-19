#ifndef _gbphoto_h__
#define _gbphoto_h__

#include <stdint.h>

struct gb_ram_header {
	uint8_t scratchpad1[0x11FC]; // 0000 - 11fb
	uint8_t gameface[0xE00]; // 11fc - 1ffb
	uint8_t scratchpad2[4];
};

struct gb_photo {
	uint8_t large[0xE00]; // x000 to xDFF
	uint8_t small[0x100]; // xE00 to xEFF
	uint8_t info[0x100]; // xF00 to xFFF
};

int gbphoto_read_header(FILE *fptr, struct gb_ram_header *dstheader);
int gbphoto_read(FILE *fptr, struct gb_photo *dstphoto);
int gbphoto_getOffset(int photo_id);
int gbphoto_isActive(const struct gb_ram_header *header, int index, int *logical_number);

#define GB_FIRST_PHOTO_OFFSET	0x2000
#define GB_PHOTO_SIZE			0x1000


#endif // _gbphoto_h__
