INCLUDES = 			\
	-DGNOMELOCALEDIR=\""$(datadir)/locale"\" \
	-I$(includedir)				 \
	$(GNOME_INCLUDEDIR)

bin_PROGRAMS = gwanderer

gwanderer_SOURCES = \
	gwanderer.c \
	gwanderer.h \
	statusbar.c \
	statusbar.h \
	gameconfig.c \
	gameconfig.h \
	menu.c \
	menu.h \
	properties.c \
	properties.h \
	graphics.c \
	graphics.h \
	keyboard.c \
	keyboard.h \
	keylabels.h \
	game.c \
	game.h \
	sound.c \
	sound.h \
	gbdefs.h \
	read.c \
	play.c \
	monsters.c \
	fall.c

gwanderer_LDADD = \
	$(GNOME_LIBDIR) \
	$(GNOMEUI_LIBS)	\
	$(INTLLIBS)
