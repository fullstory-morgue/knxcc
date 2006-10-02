AM_CPPFLAGS = $(all_includes)
METASOURCES = AUTO

kde_module_LTLIBRARIES = kcm_knxcc_network.la

kcm_knxcc_network_la_SOURCES = networkdialog.ui interfacewidget.ui knxcc_network_interface.cpp knxcc_network.cpp process.cpp interfacestack.cpp qhexvalidator.cpp ipaddressvalidator.cpp
kcm_knxcc_network_la_LDFLAGS  = $(all_libraries) -module -avoid-version -no-undefined
kcm_knxcc_network_la_LIBADD = $(LIB_KIO)

noinst_HEADERS = knxcc_network.h knxcc_network_interface.h qhexvalidator.h ipaddressvalidator.h

xdg_apps_DATA = knxcc_network.desktop

messages: rc.cpp
	$(EXTRACTRC) *.ui *.rc >> rc.cpp
	$(XGETTEXT) $(kcm_knxcc_network_la_SOURCES) rc.cpp -o $(podir)/knxccnetwork.pot
