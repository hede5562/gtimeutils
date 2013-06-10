## Process this file with automake to produce Makefile.in

## Created by Anjuta



AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\" \
	$(GTIMEUTILS_CFLAGS)

AM_CFLAGS =\
	 -Wall\
	 -g

bin_PROGRAMS = gstopwatch gtimer

gstopwatch_SOURCES = \
	gstopwatch.c

gstopwatch_LDFLAGS = \
	-Wl,--export-dynamic

gstopwatch_LDADD = $(GTIMEUTILS_LIBS)

gtimer_SOURCES = \
	gtimer.c

gtimer_LDFLAGS = \
	-Wl,--export-dynamic

gtimer_LDADD = $(GTIMEUTILS_LIBS) \
	$(libcanberra_LIBS)
