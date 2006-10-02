AM_CPPFLAGS = $(all_includes)
METASOURCES = AUTO

kde_module_LTLIBRARIES = kcm_knxcc_display.la

kcm_knxcc_display_la_SOURCES = displaydialog.ui knxcc_display.cpp process.cpp
kcm_knxcc_display_la_LDFLAGS  = $(all_libraries) -module -avoid-version -no-undefined
kcm_knxcc_display_la_LIBADD = $(LIB_KIO)

noinst_HEADERS = knxcc_display.h

xdg_apps_DATA = knxcc_display.desktop

messages: rc.cpp
	$(EXTRACTRC) *.ui *.rc >> rc.cpp
	$(XGETTEXT) $(kcm_knxcc_display_la_SOURCES) rc.cpp -o $(podir)/knxccdisplay.pot
