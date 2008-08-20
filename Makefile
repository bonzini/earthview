CFLAGS = -g `pkg-config cairo --cflags` `pkg-config sdl --cflags`
LDFLAGS = -g `pkg-config cairo --libs` `pkg-config sdl --libs`

all: earthview sunrise-test
clean:
	rm earthview sunrise-test *.o

earthview: drawing.o earthview.o
sunrise-test: sunrise-test.o sunrise.o

drawing.o: drawing.c drawing.h
earthview.o: earthview.c drawing.h
sunrise.o: sunrise.c sunrise.h
sunrise-test.o: sunrise-test.c sunrise.h

