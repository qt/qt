load(qttest_p4)

QT = core network

SOURCES  += tst_qtcpsocket_stresstest.cpp \
    minihttpserver.cpp

HEADERS += \
    minihttpserver.h

RESOURCES += wwwfiles.qrc
