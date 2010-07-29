INCLUDEPATH += $$PWD/libconninet/src

QMAKE_CXXFLAGS += $$QT_CFLAGS_GLIB

HEADERS += \
    $$PWD/libconninet/src/dbusdispatcher.h \
    $$PWD/libconninet/src/iapconf.h \
    $$PWD/libconninet/src/iapmonitor.h \
    $$PWD/libconninet/src/maemo_icd.h \
    $$PWD/libconninet/src/proxyconf.h

SOURCES += \
    $$PWD/libconninet/src/dbusdispatcher.cpp \
    $$PWD/libconninet/src/iapconf.cpp \
    $$PWD/libconninet/src/iapmonitor.cpp \
    $$PWD/libconninet/src/maemo_icd.cpp \
    $$PWD/libconninet/src/proxyconf.cpp

