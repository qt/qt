TEMPLATE = app
CONFIG += qt uic console
DESTDIR = ../../bin

QT += declarative

TARGET = qmlplugindump

SOURCES += \
    main.cpp \
    qmlstreamwriter.cpp

HEADERS += \
    qmlstreamwriter.h

OTHER_FILES += Info.plist
macx: QMAKE_INFO_PLIST = Info.plist

target.path = $$[QT_INSTALL_BINS]
INSTALLS += target
