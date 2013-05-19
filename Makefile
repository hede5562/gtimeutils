CC       = gcc
GTK     ?= 3
CFLAGS   = -std=c99 -Wall -Wextra -DGTK${GTK}

LIBS_TIMER = $(shell pkg-config --cflags --libs gtk+-${GTK}.0 libnotify libcanberra)
LIBS_STOPW = $(shell pkg-config --cflags --libs gtk+-${GTK}.0 libnotify libcanberra)

PREFIX   ?= /usr/local
BINPREFIX = $(PREFIX)/bin

all: CFLAGS += -Os
all: gstopwatch gtimer

debug: CFLAGS += -O0 -g -pedantic -DDEBUG
debug: all

gstopwatch: gstopwatch.c
	$(CC) gstopwatch.c -o gstopwatch $(CFLAGS) $(LIBS_STOPW)

gtimer: gtimer.c
	$(CC) gtimer.c -o gtimer $(CFLAGS) $(LIBS_TIMER)

install:
	mkdir -p $(DESTDIR)/$(BINPREFIX)
	mkdir -p $(DESTDIR)/usr/share/applications/

	install -m 0755 gstopwatch $(DESTDIR)/$(BINPREFIX)/
	install -m 0755 gtimer $(DESTDIR)/$(BINPREFIX)/
	install -m 0644 data/gstopwatch.desktop $(DESTDIR)/usr/share/applications/
	install -m 0644 data/gtimer.desktop $(DESTDIR)/usr/share/applications/

uninstall:
	rm -f $(BINPREFIX)/gstopwatch
	rm -f $(BINPREFIX)/gtimer
	rm -f /usr/share/applications/gstopwatch.desktop
	rm -r /usr/share/applications/gtimer.desktop

clean:
	rm -f gstopwatch
	rm -f gtimer
