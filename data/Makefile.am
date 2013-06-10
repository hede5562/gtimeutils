desktopdir = $(datadir)/applications
desktop_in_files = gstopwatch.desktop.in gtimer.desktop.in
desktop_DATA = $(desktop_in_files:.desktop.in=.desktop)
@INTLTOOL_DESKTOP_RULE@

UPDATE_DESKTOP = update-desktop-database $(datadir)/applications || :

install-data-hook:
	$(UPDATE_DESKTOP)
uninstall-hook:
	$(UPDATE_DESKTOP)
