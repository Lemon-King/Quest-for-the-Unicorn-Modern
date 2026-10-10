PROJECT = saveconvert
INSTALLDIR = /usr/local/games/
CC = gcc
CFLAGS =
LFLAGS =
INCDIR = 
INCS =

.PHONY: clean dist

# Defs for compiling with UNIX/Text
#DEFS = -DUNIX -O3 -Wall
#LIBDIR = 
#LIBS = 

# Defs for compiling with UNIX/X-Windows
DEFS = -DUNIX -O3 -Wall
LIBDIR = 
LIBS = 

# Defs for compiling with MINGW for native Windows binary
#DEFS = -DUNIX -O3 -Wall -DMINGW -UWIN32
#LIBDIR = 
#LIBS = 

# Defs for compiling with MINGW for native Windows binary graphics
#DEFS = -DUNIX -O3 -Wall -DWIN32 -DMINGW 
#LIBDIR = 
#LIBS = 

HEADERS = header.h objects.h monsters.h classes.h races.h world.h

OBJS = \
	load.o \
	load57.o \
	builddungeon.o \
	charattributes.o \
	checklevelclear.o \
	getlinkedmonst.o \
	getskill.o \
	gold.o \
	hasintrinsic.o \
	hassocket.o \
	isweapon.o \
	loadlevel.o \
	mapgen.o \
	rcsrandom.o \
	savenew.o \
	scalemonster.o \
	stockmonsters.o \
	urandom.o \
	main.o

$(PROJECT): $(OBJS)
	$(CC) $(LFLAGS) $(DEFS) $(LIBDIR) -o $(PROJECT) $(OBJS) $(LIBS)

.c.o:
	$(CC) $(CFLAGS) $(DEFS) $(INCDIR) $(INCS) -c $<

clean:
	-rm *.o
	-rm $(PROJECT)

builddungeon.o:     $(HEADERS) builddungeon.c
charattributes.o:   $(HEADERS) charattributes.c
checklevelclear.o:  $(HEADERS) checklevelclear.c
getlinkedmonst.o:   $(HEADERS) getlinkedmonst.c
getskill.o:         $(HEADERS) getskill.c
gold.o:             $(HEADERS) gold.c
hasintrinsic.o:     $(HEADERS) hasintrinsic.c
hassocket.o:        $(HEADERS) hassocket.c
isweapon.o:         $(HEADERS) isweapon.c
load.o:             $(HEADERS) load.c
load57.o:           $(HEADERS) load57.c
loadlevel.o:        $(HEADERS) loadlevel.c
main.o:             $(HEADERS) main.c
mapgen.o:           $(HEADERS) mapgen.c
rcsrandom.o:        $(HEADERS) rcsrandom.c
savenew.o:          $(HEADERS) savenew.c
scalemonster.o:     $(HEADERS) scalemonster.c
stockmonsters.o:    $(HEADERS) stockmonsters.c
urandom.o:          $(HEADERS) urandom.c
