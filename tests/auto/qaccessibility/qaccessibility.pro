load(qttest_p4)
requires(contains(QT_CONFIG,accessibility))
SOURCES  += tst_qaccessibility.cpp

linux-*:system(". /etc/lsb-release && [ $DISTRIB_CODENAME = oneiric ]"):DEFINES+=UBUNTU_ONEIRIC # QTBUG-26499

unix:!mac:LIBS+=-lm
contains(QT_CONFIG, qt3support): QT += qt3support

wince*: {
	accessneeded.files = $$QT_BUILD_TREE\\plugins\\accessible\\*.dll
	accessneeded.path = accessible
	DEPLOYMENT += accessneeded
}
