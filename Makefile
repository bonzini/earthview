CFLAGS = -g `pkg-config cairo --cflags` `pkg-config sdl --cflags` -O2 -ffast-math
LDFLAGS = -g `pkg-config cairo --libs` `pkg-config sdl --libs` -lm

all: earthview sunrise-test
clean:
	rm -f earthview sunrise-test *.o

.o:
	$(CC) -o $@ $^ $(LDFLAGS)

earthview: earthview.o anim.o map.o drawing.o sunrise.o
sunrise-test: sunrise-test.o sunrise.o

anim.o: anim.c drawing.h
map.o: map.c drawing.h sunrise.h project.h map.h anim.h
drawing.o: drawing.c drawing.h
earthview.o: earthview.c drawing.h
sunrise.o: sunrise.c sunrise.h
sunrise-test.o: sunrise-test.c sunrise.h

