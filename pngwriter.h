#ifndef _pngwriter_h__
#define _pngwriter_h__

#include "gbphoto.h"

int gbphoto_gameface_writepng(const struct gb_ram_header *header, const char *filename);
int gbphoto_writepng(const struct gb_photo *photo, const char *filename, int small_photo);

#endif // _pngwriter_h__
