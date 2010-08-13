DEFS = -DDBUS_API_SUBJECT_TO_CHANGE -DGCONF_DISABLE_DEPRECATED
INCLUDES = @GLIB_CFLAGS@ @DBUS_CFLAGS@ @GCONF_CFLAGS@ @OSSO_IC_CFLAGS@ @ICD_DEV_CFLAGS@ @QTCORE_CFLAGS@ @QTNETWORK_CFLAGS@ @QTDBUS_CFLAGS@ @QGCONF_CFLAGS@ @CONNSETTINGS_CFLAGS@
LIBS = @GLIB_LIBS@ @DBUS_LIBS@ @GCONF_LIBS@ @OSSO_IC_LIBS@ @ICD_DEV_LIBS@ @QTCORE_LIBS@ @QTDBUS_LIBS@ @QTNETWORK_CFLAGS@ @QGCONF_LIBS@ @CONNSETTINGS_LIBS@

AM_CFLAGS = $(CONCFLAGS)

lib_LTLIBRARIES = libconninet.la
libconninet_la_SOURCES = \
	maemo_icd.cpp \
	dbusdispatcher.cpp \
	iapconf.cpp \
	iapmonitor.cpp \
	proxyconf.cpp

#libconninet_la_LDFLAGS = -version-info 0:0:0

conninetincludedir = $(includedir)/conninet
conninetinclude_HEADERS =  \
	maemo_icd.h \
	dbusdispatcher.h \
	iapconf.h \
	iapmonitor.h \
	proxyconf.h

nodist_libconninet_la_SOURCES = \
	moc_maemo_icd.cpp \
	moc_dbusdispatcher.cpp

MAINTAINERCLEANFILES = Makefile.in

# This rule lets GNU make create any moc_*.cpp from the equivalent *.h
moc_%.cpp: %.h
	moc $< -o $@

# This line ensures that generated moc files are deleted when we make clean.
CLEANFILES = *_moc.cpp *.loT
