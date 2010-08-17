DEFS = -DDBUS_API_SUBJECT_TO_CHANGE
INCLUDES = @GLIB_CFLAGS@ @DBUS_CFLAGS@ @CONNSETTINGS_CFLAGS@ @OSSO_IC_CFLAGS@ @ICD_DEV_CFLAGS@ \
	@QTCORE_CFLAGS@ @QTDBUS_CFLAGS@ @QTCORE_CFLAGS@ @QTNETWORK_CFLAGS@ @QTTEST_CFLAGS@ \
	-I$(top_srcdir) \
	-I$(top_srcdir)/src

LIBS = @GLIB_LIBS@ @DBUS_LIBS@ @CONNSETTINGS_LIBS@ @OSSO_IC_LIBS@ @ICD_DEV_LIBS@ \
	@QTCORE_LIBS@ @QTDBUS_LIBS@ @QTCORE_LIBS@ @QTNETWORK_LIBS@ @QTTEST_LIBS@ \
	$(top_builddir)/src/libconninet.la

AM_CFLAGS = $(CONCFLAGS)
AM_LDFLAGS = -static

bin_PROGRAMS = test_dbusdispatcher test_iapconf test_maemo_icd test_iapmonitor test_proxyconf

test_dbusdispatcher_SOURCES = ut_dbusdispatcher.cpp
ut_dbusdispatcher.o: ut_dbusdispatcher.moc

test_iapconf_SOURCES = ut_iapconf.cpp
ut_iapconf.o: ut_iapconf.moc

test_maemo_icd_SOURCES = ut_maemo_icd.cpp
ut_maemo_icd.o: ut_maemo_icd.moc

test_iapmonitor_SOURCES = ut_iapmonitor.cpp
ut_iapmonitor.o: ut_iapmonitor.moc

test_proxyconf_SOURCES = ut_proxyconf.cpp
ut_proxyconf.o: ut_proxyconf.moc


MAINTAINERCLEANFILES = Makefile.in

# This rule lets GNU make create any *.moc from the equivalent *.cpp
%.moc: %.cpp
	moc $< -o $@

# This line ensures that generated moc files are deleted when we make clean.
CLEANFILES = *.moc
