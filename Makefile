CFLAGS = -g `pkg-config cairo --cflags` `pkg-config sdl --cflags`
LDFLAGS = -g `pkg-config cairo --libs` `pkg-config sdl --libs`

all: earthview sunrise-test
clean:
	rm earthview sunrise-test *.o

earthview: anim.o map.o drawing.o earthview.o sunrise.o
sunrise-test: sunrise-test.o sunrise.o

anim.o: anim.c drawing.h
map.o: map.c drawing.h sunrise.h project.h map.h anim.h
drawing.o: drawing.c drawing.h
earthview.o: earthview.c drawing.h
sunrise.o: sunrise.c sunrise.h
sunrise-test.o: sunrise-test.c sunrise.h

