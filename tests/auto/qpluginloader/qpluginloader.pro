QT = core
TEMPLATE    =	subdirs
CONFIG  += ordered
!integrity:SUBDIRS = lib \
                theplugin \
		tst
!win32: !macx-*: !symbian: !integrity: SUBDIRS += almostplugin
TARGET = tst_qpluginloader

# no special install rule for subdir
INSTALLS =


CONFIG += parallel_test
