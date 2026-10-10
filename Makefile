PROJECT = unicorn
CC = gcc
CFLAGS =
LFLAGS =
INCDIR =
INCS =

# Everything the build makes goes under bin/: the game, mapcnvt, the objects in bin/obj and the
# runtime files the game needs beside it
BINDIR = bin
OBJDIR = $(BINDIR)/obj

# Windows linkers add .exe, so targets must name it or make relinks them on every run
ifeq ($(OS),Windows_NT)
EXE = .exe
else
EXE =
endif

.PHONY: all clean

# Win32 GDI build (MinGW-w64)
DEFS = -O3 -Wall -DHC -DWIN32_LEAN_AND_MEAN -DNOMINMAX
LIBDIR =
LIBS = -s -lgdi32 -lcomctl32 -lwinmm -Wl,--subsystem,windows

#HEADERS = header.h maps.h objects.h world.h gen.h lore.h monsters.h recipe.h spells.h races.h classes.h trophies.h Makefile
HEADERS = header.h maps.h objects.h world.h gen.h lore.h monsters.h recipe.h spells.h races.h classes.h trophies.h terminal.h
OBJS = \
	activate.o \
	addskill.o \
	adjustabilities.o \
	adjustmap.o \
	advancement.o \
	advancestat.o \
	aoe.o \
	area9.o \
	area25.o \
	area49.o \
	arena.o \
	arenaheal.o \
	arenamoveenemy.o \
	arenamoveparty.o \
	arenamovesnake.o \
	autodetect.o \
	bank.o \
	blind.o \
	blindtrap.o \
	builddungeon.o \
	buyhouse.o \
	buyprice.o \
	cansee.o \
	castspell.o \
	cast_spell.o \
	cavalier.o \
	cave.o \
	charattributes.o \
	charmmonster.o \
	checkautos.o \
	checkclasschange.o \
	checkencounter.o \
	checkintrinsic.o \
	checklevelclear.o \
	checkmonkweapon.o \
	checkmount.o \
	checknoble.o \
	checkscores.o \
	checkspell1.o \
	cleargamedata.o \
	clearprompt.o \
	closedoor.o \
	combathelp.o \
	compass.o \
	convertrecipe.o \
	converttime.o \
	copymap.o \
	crafting.o \
	darttrap.o \
	datacard.o \
	deadmonster.o \
	decode.o \
	discover.o \
	dispchar.o \
	displayabilities.o \
	dmgreduction.o \
	doblink.o \
	doexchange.o \
	doget.o \
	doheal.o \
	dosleep.o \
	drain.o \
	dragondice.o \
	dungeonhelp.o \
	effect.o \
	enemyhit.o \
	enemymissile.o \
	exist.o \
	fear.o \
	feardir.o \
	figuredamage.o \
	findempty.o \
	findgen.o \
	finditemslot.o \
	findlinkeditem.o \
	findlore.o \
	findpath.o \
	findtarget.o \
	fire.o \
	firetrap.o \
	firewand.o \
	fireweapon.o \
	fixability.o \
	fixname.o \
	fkeys.o \
	gainexp.o \
	gamble.o \
	gameoptions.o \
	getalign.o \
	getcity.o \
	getclass.o \
	getdir.o \
	getdungeon.o \
	gethealth.o \
	getitem.o \
	getlinkedmonst.o \
	getlinkedmsg.o \
	getmonster.o \
	getname.o \
	getrace.o \
	getsex.o \
	getskill.o \
	getshopitems.o \
	getshoprecipes.o \
	getshopspells.o \
	getspellname.o \
	gobottom.o \
	godown.o \
	gold.o \
	gothicfont.o \
	goship.o \
	goup.o \
	graphics.o \
	haggle.o \
	hasintrinsic.o \
	hassocket.o \
	hasspell.o \
	healing.o \
	holeinventory.o \
	houseinventory.o \
	inkey.o \
	inroom.o \
	insight.o \
	inventory.o \
	invvalid.o \
	intrinsiccount.o \
	isweapon.o \
	itemcolor.o \
	itemcost.o \
	itemdir.o \
	levellegacy.o \
	library.o \
	lightroom.o \
	load.o \
	loadgear.o \
	loadlevel.o \
	loadmenu.o \
	log.o \
	lockdoor.o \
	los.o \
	magicadjust.o \
	main.o \
	make_a.o \
	make_c.o \
	make_h.o \
	make_n.o \
	makechar.o \
	makemap.o \
	makestr.o \
	manacost.o \
	mapgen.o \
	mapgenold.o \
	maxdamage.o \
	mechmerchant.o \
	meleehit.o \
	missile.o \
	mounts.o \
	moveenemy.o \
	movesnake.o \
	movesummoned.o \
	msg.o \
	music.o \
	mutable.o \
	mutalegend.o \
	mval.o \
	nap.o \
	needed.o \
	newgame.o \
	newmonth.o \
	newpawn.o \
	newspell.o \
	noise.o \
	opendoor.o \
	packit.o \
	packitem.o \
	party.o \
	passable.o \
	pawn.o \
	pittrap.o \
	placeobject.o \
	poisontrap.o \
	possesses.o \
	printrec.o \
	processconfig.o \
	processoption.o \
	prompt.o \
	r_missile.o \
	rcsrandom.o \
	readsprites.o \
	recipestring.o \
	reducedmg.o \
	removeplayer.o \
	removelandplayer.o \
	reversebyte.o \
	roottrap.o \
	savenew.o \
	savegear.o \
	savelevel.o \
	sayword.o \
	scalemonster.o \
	school.o \
	selectsprite.o \
	settitle.o \
	settrap.o \
	shop.o \
	showalign.o \
	showheadings.o \
	showhealth.o \
	showfkeys.o \
	showflt.o \
	showgain.o \
	showgen.o \
	showinventory.o \
	showicons.o \
	showint.o \
	showland.o \
	showlandplayer.o \
	showlog.o \
	showlore.o \
	showmap.o \
	showmessages.o \
	shownumber.o \
	showplayer.o \
	showquest.o \
	showscores.o \
	showscreen.o \
	showside.o \
	showsmland.o \
	showsmmap.o \
	showspell.o \
	showspot.o \
	showsprite.o \
	showstats.o \
	sleeptrap.o \
	sortshopitems.o \
	specials.o \
	sqr.o \
	statsdodamage.o \
	statstakedamage.o \
	stockencounter.o \
	stockmonsters.o \
	stocktown.o \
	stocktreasure.o \
	strokefont.o \
	super.o \
	taketime.o \
	target.o \
	tavern.o \
	teleport.o \
	teleporttrap.o \
	temple.o \
	terminal.o \
	throw.o \
	thrownpotion.o \
	title.o \
	touch.o \
	transcribe.o \
	travel.o \
	triggertrap.o \
	trophies.o \
	trunc.o \
	unpack.o \
	upland.o \
	uplandhelp.o \
	upper.o \
	urandom.o \
	urandomize.o \
	usecompass.o \
	useitem.o \
	validmove.o \
	view.o \
	winner.o \
	wizard.o \
	writefont.o \
	yesno.o

GAME = $(BINDIR)/$(PROJECT)$(EXE)
MAPCNVT = $(BINDIR)/mapcnvt$(EXE)

# The game reads these from its working directory, so they sit beside it
RUNTIME = $(BINDIR)/unicorn.cfg $(BINDIR)/unicorn.doc
MUSIC = $(addprefix $(BINDIR)/music/,intro.mid upland.mid dungeon.mid town.mid)

all: $(GAME) $(RUNTIME) $(MUSIC)

$(GAME): $(addprefix $(OBJDIR)/,$(OBJS))
	$(CC) $(LFLAGS) $(DEFS) $(LIBDIR) -o $@ $^ $(LIBS)

$(RUNTIME): $(BINDIR)/%: % | $(OBJDIR)
	cp $< $@

$(MUSIC): $(BINDIR)/music/%: music/% | $(BINDIR)/music
	cp $< $@

$(BINDIR)/music:
	mkdir -p $@

maps.h: $(MAPCNVT) map11.map map12.map map13.map map14.map map21.map map22.map map23.map map24.map map31.map map32.map map33.map map34.map eshter.map generic.map radon.map marduk.map nichor.map village.map
	$(MAPCNVT) > maps.h

$(MAPCNVT): $(OBJDIR)/mapcnvt.o
	$(CC) $(LFLAGS) $(LIBDIR) $(LIBS) -o $@ $<

# mapcnvt writes maps.h, so its object must not depend on the headers that include it
$(OBJDIR)/mapcnvt.o: mapcnvt.c | $(OBJDIR)
	$(CC) $(CFLAGS) $(DEFS) $(INCDIR) $(INCS) -c $< -o $@

$(OBJDIR)/%.o: %.c $(HEADERS) | $(OBJDIR)
	$(CC) $(CFLAGS) $(DEFS) $(INCDIR) $(INCS) -c $< -o $@

$(OBJDIR)/graphics.o: rgb.h graphics.h
$(OBJDIR)/terminal.o: graphics.h
$(OBJDIR)/selectsprite.o: unicorn.xpm
$(OBJDIR)/gothicfont.o: gothic.h

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	-rm -rf $(BINDIR)

