# Game Boy Camera save RAM photo to PNG

This is a command-line tool for extracting
Game Boy Camera photos from a save ram (.sav) file
and writing them in the PNG format.

Features:

* Extract "large" photos (128x112) to .PNG
* Extract "small" (thumbnail) photos to PNG
* Extract the "Game Faces" photo to a .PNG
* Recover deleted photos
* Single photo or batch (all photos) extraction
* List photos

The project homepage is located at: https://www.raphnet.net/programmation/gbcam2png/index_en.php

** A version for your browser **

A limited version (currently only shows "large" photos) written in Javascript/HTML5 is also
also included. It renders photos in Canvas elements which can then be saved by right-clicking
and choosing "Save as...". This version should run on any OS where a modern browser is available,
without using the command-line and without compiling anything.

[Try it right here!][https://www.raphnet.net/programmation/gbcam2png/gbcamtool.html]

## Use

### Help (-h)

 Options:
	-h              Display this help text
	-i index        Source image index (first image is 0). Default: 0
	-g              Use gameface as source photo
	-o output.png   Export a single photo to a .png file
	-b basename     Export all photos to png files, using the specified basename
	-d              Display selected photo (see -i) to the terminal
	-l              List photos in save file
	-a              Also export deleted photos
	-s              Export/display small photos (32x32) intead of large photos (128x112)
	-v              Be verbose


### Examples

#### Listing photos (shows which one are active and deleted) ####

```./gbcam2png source.sav -l```

#### Exporting a single photo (index 0) to a .png file ####

```./gbcam2png source.sav -i 0 -o output.png```

#### Exporting all active photos ####

```./gbcam2png source.sav -b test```

For each active photo, a file named testXX.png (where XX corresponds
to the photo index, starting at 00) will be created.

#### Exporting all photos, including deleted ones ####

```./gbcam2png source.sav -b test -a```


## Compilation

### Linux

Install the libpng development package beforehand, and make sure pkg-config is in your
path. Then type ```make```.

## License

Public Domain

## Authors

* **Raphael Assenat**

## Acknowledgments

* Jeff F : For the "GB Camera Tech Info" (gbcam.txt) document
