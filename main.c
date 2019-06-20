#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include "gbphoto.h"
#include "pngwriter.h"

static uint8_t *getTile(uint8_t *tiledata, int tile_id)
{
	return tiledata + 16 * tile_id;
}

static void outputPhotoToTerminal(struct gb_photo *photo, int small_photo)
{
	uint8_t *p;
	int x,y;
	int w_tiles = small_photo ? 4 : 16;
	int h_tiles = small_photo ? 4 : 14;
	const char *palette[5] = { "\033[107;37;1m#", "\033[0;47;37mo", "\033[100;90m.", "\033[0;30m ", "\033[0m" };
	int i;

	for (y=0; y<h_tiles*8; y++) {

		// The small picture is 4x4 tiles (32x32 pixels) but the actual
		// picture is 32x28. A white band apperas at the bottom without
		// this. In the game UI they are displayed on a white background
		// so it does not matter...
		if (small_photo && (y >= 28))
			break;

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
				printf("%s", palette[val]);
			}

		}
		printf("%s\n", palette[4]);
	}
}

static void printHelp(void)
{
	printf("Usage: ./gbcam2png savefile [options]\n");
	printf("\n");
	printf("Options:\n");
	printf("  -h              Display this help text\n");
	printf("  -i index        Source image index (first image is 0). Default: 0\n");
	printf("  -g              Use gameface as source photo\n");
	printf("  -o output.png   Export a single photo to a .png file\n");
	printf("  -b basename     Export all photos to png files, using the specified basename\n");
	printf("  -d              Display selected photo (see -i) to the terminal\n");
	printf("  -l              List photos in save file\n");
	printf("  -a              Also export deleted photos\n");
	printf("  -s              Export/display small photos (32x32) intead of large photos (128x112)\n");
	printf("  -v              Be verbose\n");
	printf("\n\n");
	printf("Examples:\n");
	printf("\n");
	printf(" ** Listing photos (shows which one are active and deleted)\n\n");
	printf("  ./gbcam2png source.sav -l\n");
	printf("\n");
	printf(" ** Exporting a single photo (index 0) to a .png file\n\n");
	printf("  ./gbcam2png source.sav -i 0 -o output.png\n");
	printf("\n");
	printf(" ** Exporting all active photos:\n\n");
	printf("  ./gbcam2png source.sav -b test\n\n");
	printf(" For each active photo, a file named testXX.png (where XX corresponds\n");
	printf(" to the photo index, starting at 00) will be created.\n\n");
	printf(" ** Exporting all photos, including deleted ones:\n\n");
	printf("  ./gbcam2png source.sav -b test -a\n\n");

}

int main(int argc, char **argv)
{
	FILE *fptr;
	struct gb_ram_header header;
	struct gb_photo photo;
	int res;
	const char *input_save_file;
	int arg_image_index = 0;
	const char *arg_output_filename = NULL;
	const char *arg_basename = NULL;
	int arg_display_terminal = 0;
	int arg_list_photos = 0;
	int arg_include_deleted = 0;
	int arg_verbose = 0;
	int arg_use_small_photos = 0;
	int arg_use_gameface = 0;
	int i;

	while ((res = getopt(argc, argv, "hi:o:b:dlavsg")) != -1) {
		switch (res)
		{
			case 'h':
				printHelp();
				return 0;
			case 'i':
				arg_image_index = atoi(optarg);
				break;
			case 'o':
				arg_output_filename = optarg;
				break;
			case 'b':
				arg_basename = optarg;
				break;
			case 'd':
				arg_display_terminal = 1;
				break;
			case 'l':
				arg_list_photos = 1;
				break;
			case 'a':
				arg_include_deleted = 1;
				break;
			case 'v':
				arg_verbose = 1;
				break;
			case 's':
				arg_use_small_photos = 1;
				break;
			case 'g':
				arg_use_gameface = 1;
				break;
			default:
				fprintf(stderr, "Unknown option. Try -h\n");
				return 1;
		}
	}

	if (optind >= argc) {
		fprintf(stderr, "No input save file specified\n");
		return 1;
	}

	input_save_file = argv[optind];

	// Argument sanity checks
	if (!arg_basename && !arg_output_filename && !arg_display_terminal && !arg_list_photos) {
		fprintf(stderr, "Nothing to do. Try -h\n");
		return 1;
	}
	if (arg_use_small_photos && arg_use_gameface) {
		fprintf(stderr, "-s and -g cannot be used together. The gameface photo does not have a small version.\n");
		return 1;
	}

	fptr = fopen(input_save_file, "rb");
	if (!fptr) {
		perror("fopen");
		return -1;
	}

	res = gbphoto_read_header(fptr, &header);
	if (res) {
		return -1;
	}

	if (arg_list_photos || arg_basename) {

		for (i=0; i<30; i++) {
			res = fseek(fptr, gbphoto_getOffset(i), SEEK_SET);
			if (res) {
				perror("fseek");
				fclose(fptr);
				return -1;
			}
			res = gbphoto_read(fptr, &photo);
			if (res) {
				fclose(fptr);
				return -1;
			}

			if (arg_list_photos) {
				int logical;

				printf("Photo index %d: ", i);

				if (gbphoto_isActive(&header, i, &logical)) {
					printf("Active (Logical photo number %d)", logical);
				}
				else {
					printf("Deleted");
				}

				printf("\n");
			}

			if (arg_basename) {
				char tmpfilename[strlen(arg_basename)+2+4+1];

				if (gbphoto_isActive(&header, i, NULL) || arg_include_deleted) {
					snprintf(tmpfilename, sizeof(tmpfilename), "%s%02d.png", arg_basename, i);

					if (arg_verbose) {
						printf("Writing file %s\n", tmpfilename);
					}

					gbphoto_writepng(&photo, tmpfilename, arg_use_small_photos);
				} else {
					if (arg_verbose) {
						printf("Skipping deleted photo %d\n", i);
					}
				}
			}
		}
	}
	else
	{
		if (arg_use_gameface) {
			// instead of loading a photo, build one from the gameface
			// in the header.
			memcpy(photo.large, header.gameface, sizeof(header.gameface));
		} else {
			res = fseek(fptr, gbphoto_getOffset(arg_image_index), SEEK_SET);
			if (res) {
				perror("fseek");
				fclose(fptr);
				return -1;
			}
			res = gbphoto_read(fptr, &photo);
			if (res) {
				fclose(fptr);
				return -1;
			}
		}
		if (arg_display_terminal) {
			outputPhotoToTerminal(&photo, arg_use_small_photos);
		}
		if (arg_output_filename) {
			gbphoto_writepng(&photo, arg_output_filename, arg_use_small_photos);
		}
	}

	fclose(fptr);

	return 0;
}
