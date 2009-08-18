TARGET = QtTest
QPRO_PWD = $$PWD
QT = core
INCLUDEPATH += .
unix:!embedded:QMAKE_PKGCONFIG_DESCRIPTION = Qt \
    Unit \
    Testing \
    Library

# Input
HEADERS = qtest_global.h \
    qtestcase.h \
    qtestdata.h \
    qtesteventloop.h \
    qtestcorelist.h \
    qtestcoreelement.h \
    qtestelement.h \
    qtestelementattribute.h \
    qtestbasicstreamer.h \
    qtestxunitstreamer.h \
    qtestxmlstreamer.h \
    qtestlightxmlstreamer.h \
    qtestfilelogger.h \
    qtesttouch.h
SOURCES = qtestcase.cpp \
    qtestlog.cpp \
    qtesttable.cpp \
    qtestdata.cpp \
    qtestresult.cpp \
    qasciikey.cpp \
    qplaintestlogger.cpp \
    qxmltestlogger.cpp \
    qsignaldumper.cpp \
    qabstracttestlogger.cpp \
    qbenchmark.cpp \
    qbenchmarkmeasurement.cpp \
    qbenchmarkvalgrind.cpp \
    qbenchmarkevent.cpp \
    qtestelement.cpp \
    qtestelementattribute.cpp \
    qtestbasicstreamer.cpp \
    qtestxunitstreamer.cpp \
    qtestxmlstreamer.cpp \
    qtestlightxmlstreamer.cpp \
    qtestlogger.cpp \
    qtestfilelogger.cpp
DEFINES *= QT_NO_CAST_TO_ASCII \
    QT_NO_CAST_FROM_ASCII \
    QTESTLIB_MAKEDLL \
    QT_NO_DATASTREAM
embedded:QMAKE_CXXFLAGS += -fno-rtti
wince*::LIBS += libcmt.lib \
    corelibc.lib \
    ole32.lib \
    oleaut32.lib \
    uuid.lib \
    commctrl.lib \
    coredll.lib \
    winsock.lib
mac:LIBS += -framework IOKit \
    -framework ApplicationServices \
    -framework Security
include(../qbase.pri)
QMAKE_TARGET_PRODUCT = QTestLib
QMAKE_TARGET_DESCRIPTION = Qt \
    Unit \
    Testing \
    Library

symbian:TARGET.UID3=0x2001B2DF
