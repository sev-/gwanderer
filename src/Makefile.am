
INCLUDES = -I$(top_srcdir) -I$(includedir) $(GNOME_INCLUDEDIR) \
         -DG_LOG_DOMAIN=\"GnomeHello\" -DGNOMELOCALEDIR=\""$(datadir)/locale"\" \
	-I../intl -I$(top_srcdir)/intl

bin_PROGRAMS = gwanderer

gwanderer_SOURCES =     \
        app.c           \
        gwanderer.c     \
        menus.c		\
	app.h		\
	gwanderer.h	\
	menus.h		\
	graphics.c	\
	graphics.h

gwanderer_LDADD = $(GNOMEUI_LIBS) $(GNOME_LIBDIR) $(INTLLIBS)
