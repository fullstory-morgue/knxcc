AM_CPPFLAGS = $(all_includes)
METASOURCES = AUTO

kde_module_LTLIBRARIES = kcm_knxcc_time.la

kcm_knxcc_time_la_SOURCES = knxcc_time.cpp
kcm_knxcc_time_la_LDFLAGS  = $(all_libraries) -module -avoid-version -no-undefined
kcm_knxcc_time_la_LIBADD = $(LIB_KUTILS)

noinst_HEADERS = knxcc_time.h

xdg_apps_DATA = knxcc_time.desktop

messages: rc.cpp
	$(EXTRACTRC) *.ui *.rc >> rc.cpp
	$(XGETTEXT) $(kcm_knxcc_time_la_SOURCES) rc.cpp -o $(podir)/knxcctime.pot
