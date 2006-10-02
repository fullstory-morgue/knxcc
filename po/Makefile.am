POFILES = AUTO
SUBDIRS = de
SHELL = /bin/sh

merge:
	for subdir in $(SUBDIRS); do \
	  for potfile in *.pot; do \
	    if test "$$subdir" = "."; then :; else \
	      subname=$$(basename $$potfile .pot); \
	      $(MAKE) -f $(top_srcdir)/admin/Makefile.common package-merge POFILES="$$subdir/$$subname.po" PACKAGE="$$subname"; \
	    fi; \
	  done; \
	done;
