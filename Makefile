CC=gcc
LD=$(CC)

CFLAGS=-Wall -O1 `libpng-config --cflags`
LDFLAGS=`libpng-config --libs`

all: gbcam2png

gbcam2png: main.o gbphoto.o pngwriter.o
	$(LD) $(LDFLAGS) -o $@ $^ -lpng

clean:
	rm *.o gbcam2png -f
