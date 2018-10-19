#include <stdio.h>
#include "gbphoto.h"

int gbphoto_read_header(FILE *fptr, struct gb_ram_header *dstheader)
{
	int res;

	res = fread(dstheader->scratchpad1, sizeof(dstheader->scratchpad1), 1, fptr);
	if (res != 1) {
		perror("fread");
		return -1;
	}

	res = fread(dstheader->gameface, sizeof(dstheader->gameface), 1, fptr);
	if (res != 1) {
		perror("fread");
		return -1;
	}

	res = fread(dstheader->scratchpad2, sizeof(dstheader->scratchpad2), 1, fptr);
	if (res != 1) {
		perror("fread");
		return -1;
	}


	return 0;
}

int gbphoto_read(FILE *fptr, struct gb_photo *dstphoto)
{
	int res;

	res = fread(dstphoto->large, sizeof(dstphoto->large), 1, fptr);
	if (res != 1) {
		perror("fread");
		return -1;
	}

	res = fread(dstphoto->small, sizeof(dstphoto->small), 1, fptr);
	if (res != 1) {
		perror("fread");
		return -1;
	}

	res = fread(dstphoto->info, sizeof(dstphoto->info), 1, fptr);
	if (res != 1) {
		perror("fread");
		return -1;
	}

	return 0;
}

int gbphoto_getOffset(int photo_id)
{
	return GB_FIRST_PHOTO_OFFSET + photo_id * GB_PHOTO_SIZE;
}

int gbphoto_isActive(const struct gb_ram_header *header, int index, int *logical_number)
{
	uint8_t status;

	if (index >= 30) {
		return 0;
	}

	status = header->scratchpad1[0x11B2 + index];
	if (status == 0xFF) {
		return 0; // deleted
	}

	// The logical number tells the position where
	// the photo would be shown in the game UI. The photos
	// are not always stored in the same order, in particular
	// when deleting and taking new photos (new photos always
	// appear last in the game, but they will be stored
	// in any free slot.
	//
	if (logical_number) {
		*logical_number = status;
	}

	return 1;
}

