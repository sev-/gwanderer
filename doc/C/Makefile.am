gwanderer_helpdir = $(datadir)/gnome/help/gwanderer/C

gwanderer_help_DATA =         \
        gwanderer.html        \
        topic.dat

SGML_FILES =                    \
        gwanderer.sgml

# files that aren't in a binary/data/library target have to be listed here
# to be included in the tarball when you 'make dist'
EXTRA_DIST = \
        topic.dat               \
        $(SGML_FILES)


## The - before the command means to ignore it if it fails.  that way
## people can still build the software without the docbook tools

all: 

gwanderer.html: gwanderer/gwanderer.html
	-cp gwanderer/gwanderer.html .

gwanderer/gwanderer.html: $(SGML_FILES)
	-db2html gwanderer.sgml

## when we make dist, we include the generated HTML so people don't
## have to have the docbook tools
dist-hook:
	mkdir $(distdir)/gwanderer
	-cp gwanderer/*.html gwanderer/*.css $(distdir)/gwanderer
	-cp gwanderer.html $(distdir)

install-data-local: gwanderer.html
	$(mkinstalldirs) $(gwanderer_helpdir)/images
	-for file in $(srcdir)/gwanderer/*.html $(srcdir)/gwanderer/*.css; do \
	basefile=`basename $$file`; \
	$(INSTALL_DATA) $(srcdir)/$$file $(gwanderer_helpdir)/$$basefile; \
	done

gwanderer.ps: gwanderer.sgml
	-db2ps $<

gwanderer.rtf: gwanderer.sgml       
	-db2rtf $<

