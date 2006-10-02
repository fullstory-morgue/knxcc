AM_CPPFLAGS = $(all_includes)
METASOURCES = AUTO

kde_module_LTLIBRARIES = kcm_knxcc_server.la

kcm_knxcc_server_la_SOURCES = serverdialog.ui knxcc_server.cpp process.cpp
kcm_knxcc_server_la_LDFLAGS  = $(all_libraries) -module -avoid-version -no-undefined
kcm_knxcc_server_la_LIBADD = $(LIB_KIO)

noinst_HEADERS = knxcc_server.h

xdg_apps_DATA = knxcc_server.desktop

messages: rc.cpp
	$(EXTRACTRC) *.ui *.rc >> rc.cpp
	$(XGETTEXT) $(kcm_knxcc_server_la_SOURCES) rc.cpp -o $(podir)/knxccserver.pot
