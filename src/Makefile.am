AUTOMAKE_OPTIONS = 1.5 foreign subdir-objects
ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = raycast

EXTRA_DIST = run.sh.in
CLEANFILES = run.sh
noinst_SCRIPTS = run.sh

raycast_LDFLAGS = @SDL_LIBS@ @PANGOCAIRO_LIBS@
raycast_CFLAGS = @WARNING_CFLAGS@ @SDL_CFLAGS@ @PANGOCAIRO_CFLAGS@
raycast_SOURCES = \
	defs.h \
	draw.h draw_rect.c \
	imath.c imath.h \
	input.c input.h \
	level.c level.h \
	main.c \
	text.h text.c \
	texture.h texture_jpeg.c texture_make.c \
	world.h world.c

run.sh : $(srcdir)/run.sh.in Makefile
	sed \
		-e 's,@''abs_top_srcdir@,@abs_top_srcdir@,g' \
		-e 's,@''abs_top_builddir@,@abs_top_builddir@,g' \
		-e 's,@''shell@,@shell@,g' \
		$(srcdir)/run.sh.in > $@.tmp
	chmod +x $@.tmp
	mv $@.tmp $@
