AM_CPPFLAGS = $(all_includes)
METASOURCES = AUTO

kde_module_LTLIBRARIES = kcm_knxcc_boot.la

kcm_knxcc_boot_la_SOURCES = bootdialog.ui knxcc_boot.cpp process.cpp
kcm_knxcc_boot_la_LDFLAGS  = $(all_libraries) -module -avoid-version -no-undefined
kcm_knxcc_boot_la_LIBADD = $(LIB_KIO)

noinst_HEADERS = knxcc_boot.h

xdg_apps_DATA = knxcc_boot.desktop

messages: rc.cpp
	$(EXTRACTRC) *.ui *.rc >> rc.cpp
	$(XGETTEXT) $(kcm_knxcc_boot_la_SOURCES) rc.cpp -o $(podir)/knxccboot.pot
