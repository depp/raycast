AUTOMAKE_OPTIONS = 1.5 foreign subdir-objects
ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = raycast

raycast_LDFLAGS = @SDL_LIBS@
raycast_CFLAGS = @WARNING_CFLAGS@ @SDL_CFLAGS@
raycast_SOURCES = \
	defs.h \
	draw.h draw_rect.c \
	imath.c imath.h \
	level.c level.h \
	main.c \
	texture.h texture_jpeg.c texture_make.c
