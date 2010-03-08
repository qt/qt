TEMPLATE = app
TARGET = objectlistmodel
DEPENDPATH += .
INCLUDEPATH += .
QT += declarative

# Input
SOURCES += main.cpp \
           dataobject.cpp
HEADERS += dataobject.h
RESOURCES += objectlistmodel.qrc

sources.files = $$SOURCES $$HEADERS $$RESOURCES objectlistmodel.pro view.qml
sources.path = $$[QT_INSTALL_EXAMPLES]/declarative/objectlistmodel
target.path = $$[QT_INSTALL_EXAMPLES]/declarative/objectlistmodel

INSTALLS += sources target

