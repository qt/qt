TEMPLATE = app
TARGET = BearerEx

QT += core \
      gui \
      network

FORMS += sessiondialog.ui \
         bearerex.ui \
         detailedinfodialog.ui
include(../../common.pri)
#not really a test case but deployment happens same way
CONFIG += testcase

DEPENDPATH += .
INCLUDEPATH += . \
               ../../src/bearer

# Example headers and sources
HEADERS += bearerex.h \
           xqlistwidget.h
    
SOURCES += bearerex.cpp \
           main.cpp \
           xqlistwidget.cpp

symbian: {
    bearerex.sources = Qtbearer.dll
    bearerex.path = /sys/bin
    DEPLOYMENT += bearerex

    TARGET.CAPABILITY = NetworkServices NetworkControl ReadUserData
}
qtAddLibrary(QtBearer)
