DESTDIR = ../../bin

include(qmldebugger.pri)

target.path=$$[QT_INSTALL_BINS]
INSTALLS += target

CONFIG += console
