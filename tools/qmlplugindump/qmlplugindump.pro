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

# Build debug and release versions of the tool on Windows -
# if debug and release versions of Qt have been built.
!build_pass:win32 {
    CONFIG -= debug release debug_and_release build_all

    contains(QT_CONFIG,debug):contains(QT_CONFIG,release) {
        CONFIG += debug_and_release build_all
    } else {
        contains(QT_CONFIG,debug): CONFIG += debug
        contains(QT_CONFIG,release): CONFIG += release
    }
}

CONFIG(debug, debug|release) {
    win32: TARGET = $$join(TARGET,,,d)
}

target.path = $$[QT_INSTALL_BINS]
INSTALLS += target
