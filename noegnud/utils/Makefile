CFLAGS = -c -Wall -g
SDLCONFIG = sdl-config
SDLCFLAGS = `$(SDLCONFIG) --cflags`
SDLLIBS = `$(SDLCONFIG) --libs`
NOEGNUDOBJDIR = ../../../../nethack/src/

EXECS = noegnud_fnt2tga \
	noegnud_tiles_split \
	noegnud_tiles_join \
	noegnud_lower

all: noegnud $(EXECS)

noegnud:
	@make -C ../../../../ nethack/Makefile nethack/win/noegnud
	@make -C ../../../../nethack/

clean:
	rm -f *.o $(EXECS)

distclean: clean
	@make -C ../ clean

spotless: clean
	@make -C ../ spotless

noegnud_fnt2tga: noegnud_fnt2tga.o
	gcc -o noegnud_fnt2tga noegnud_fnt2tga.o $(NOEGNUDOBJDIR)noegnud_gff_tga.o

noegnud_fnt2tga.o: noegnud_fnt2tga.c
	gcc $(CFLAGS) -o noegnud_fnt2tga.o noegnud_fnt2tga.c

noegnud_tiles_split: noegnud_tiles_split.o
	gcc -o noegnud_tiles_split noegnud_tiles_split.o $(NOEGNUDOBJDIR)noegnud_collection.o $(NOEGNUDOBJDIR)noegnud_typeloader.o $(NOEGNUDOBJDIR)noegnud_gff_tga.o $(NOEGNUDOBJDIR)noegnud_mem.o $(NOEGNUDOBJDIR)noegnud_debug.o $(SDLLIBS) -lSDL_image -lpng -lz

noegnud_tiles_split.o: noegnud_tiles_split.c
	gcc $(CFLAGS) -o noegnud_tiles_split.o noegnud_tiles_split.c $(SDLCFLAGS)

noegnud_tiles_join: noegnud_tiles_join.o
	gcc -o noegnud_tiles_join noegnud_tiles_join.o $(NOEGNUDOBJDIR)noegnud_collection.o $(NOEGNUDOBJDIR)noegnud_typeloader.o $(NOEGNUDOBJDIR)noegnud_gff_tga.o $(NOEGNUDOBJDIR)noegnud_mem.o $(NOEGNUDOBJDIR)noegnud_debug.o $(SDLLIBS) -lSDL_image -lpng -lz

noegnud_tiles_join.o: noegnud_tiles_join.c
	gcc $(CFLAGS) -o noegnud_tiles_join.o noegnud_tiles_join.c $(SDLCFLAGS)

noegnud_lower: noegnud_lower.o
	gcc -o noegnud_lower noegnud_lower.c

noegnud_lower.o: noegnud_lower.c
	gcc $(CFLAGS) -o noegnud_lower.o noegnud_lower.c

