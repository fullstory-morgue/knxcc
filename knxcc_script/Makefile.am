sharedir = $(pkgdatadir)
bin_SCRIPTS = knxcc

install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(sharedir)
	cp -aR $(srcdir)/modules $(DESTDIR)$(sharedir)/

messages:
	$(XGETTEXT) -Lshell knxcc modules/*.bm modules/interface -o $(podir)/knxccscript.pot
