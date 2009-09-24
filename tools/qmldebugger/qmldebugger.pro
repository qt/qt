DESTDIR = ../../bin
QT += network declarative
contains(QT_CONFIG, opengles2)|contains(QT_CONFIG, opengles1): QT += opengl

# Input
HEADERS += canvasframerate.h \
           watchtablemodel.h \
           propertyview.h \
           engine.h
SOURCES += main.cpp \
           canvasframerate.cpp \
           watchtablemodel.cpp \
           propertyview.cpp \
           engine.cpp
RESOURCES += qmldebugger.qrc

OTHER_FILES += engines.qml

target.path=$$[QT_INSTALL_BINS]
INSTALLS += target

CONFIG += console
