TEMPLATE = lib
TARGET = QtFxSupport
DEPENDPATH += .
INCLUDEPATH += .
MOC_DIR = .moc
OBJECTS_DIR = .obj
DESTDIR = ../../lib
QT += script declarative

HEADERS += contact.h contactmodel.h
SOURCES += contact.cpp contactmodel.cpp

