#include <setjmp.h>
#include <string.h>
#include "png.h"
#include "pngwriter.h"

static jmp_buf jmpbuf;

static const uint8_t *getTile(const uint8_t *tiledata, int tile_id)
{
	return tiledata + 16 * tile_id;
}

static void writepng_error_handler(png_structp png_ptr, png_const_charp msg)
{
	fprintf(stderr, "writepng libpng error: %s\n", msg);
	fflush(stderr);

	longjmp(jmpbuf, 1);
}

int gbphoto_gameface_writepng(const struct gb_ram_header *header, const char *filename)
{
	struct gb_photo photo;

	// Convert the large photo from the header to someting gbphoto_writepng accepts.
	memcpy(photo.large, header->gameface, sizeof(header->gameface));

	return gbphoto_writepng(&photo, filename, 0);
}

int gbphoto_writepng(const struct gb_photo *photo, const char *filename, int small_photo)
{
	png_structp  png_ptr;
	png_infop  info_ptr;
	int color_type = PNG_COLOR_TYPE_PALETTE;
	int w = small_photo ? 32 : 128;
	int h = small_photo ? 32 : 112;
	png_color palette[4] = {
		{ .red = 0xFF, .green = 0xFF, .blue = 0xFF },
		{ .red = 0xAA, .green = 0xAA, .blue = 0xAA },
		{ .red = 0x55, .green = 0x55, .blue = 0x55 },
		{ .red = 0, .green = 0, .blue = 0 }
	};
	uint8_t rowdata[w];
	int w_tiles = w / 8;
	int h_tiles = h / 8;
	int x, y, i;
	FILE *fptr;

	fptr = fopen(filename, "wb");
	if (!fptr) {
		perror(filename);
		return -1;
	}

	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, writepng_error_handler, NULL);
	if (!png_ptr) {
		fclose(fptr);
		return 4;   /* out of memory */
	}

	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr) {
		png_destroy_write_struct(&png_ptr, NULL);
		fclose(fptr);
		return 4;
	}

	if (setjmp(jmpbuf)) {
		png_destroy_write_struct(&png_ptr, &info_ptr);
		fclose(fptr);
		return 2;
	}

	png_init_io(png_ptr, fptr);

	png_set_IHDR(png_ptr, info_ptr, w, small_photo ? 28 : h, 8, color_type, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

	png_set_PLTE(png_ptr, info_ptr, palette, 4);

	png_write_info(png_ptr, info_ptr);

	for (y=0; y<h_tiles*8; y++) {
		uint8_t *dst = rowdata;
		const uint8_t *p;

		// The small picture is 4x4 tiles (32x32 pixels) but the actual
		// picture is 32x28. A white band apperas at the bottom without
		// this. In the game UI they are displayed on a white background
		// so it does not matter...
		if (small_photo && (y >= 28)) {
			break;
		}

		for (x=0; x<w_tiles; x++) {

			if (small_photo) {
				p = getTile(photo->small, y/8 * 0x4 + x);
			} else {
				p = getTile(photo->large, y/8 * 0x10 + x);
			}

			// skip to current line
			p += (y&7)*2;

			for (i=0; i<8; i++) {
				uint8_t val = 0;

				if (p[0] & (0x80 >> i)) {
					val += 1;
				}
				if (p[1] & (0x80 >> i)) {
					val += 2;
				}
				*dst++ = val;
			}
		}
		png_write_row(png_ptr, rowdata);
	}

	png_write_end(png_ptr, NULL);

	png_destroy_write_struct(&png_ptr, &info_ptr);

	fclose(fptr);

	return 0;
}
