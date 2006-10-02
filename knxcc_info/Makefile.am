AM_CPPFLAGS = $(all_includes)
METASOURCES = AUTO

kde_module_LTLIBRARIES = kcm_knxcc_info.la

kcm_knxcc_info_la_SOURCES = infodialog.ui knxcc_info.cpp process.cpp
kcm_knxcc_info_la_LDFLAGS  = $(all_libraries) -module -avoid-version -no-undefined
kcm_knxcc_info_la_LIBADD = $(LIB_KIO)

noinst_HEADERS = knxcc_info.h

xdg_apps_DATA = knxcc_info.desktop

messages: rc.cpp
	$(EXTRACTRC) *.ui *.rc >> rc.cpp
	$(XGETTEXT) $(kcm_knxcc_info_la_SOURCES) rc.cpp -o $(podir)/knxccinfo.pot
