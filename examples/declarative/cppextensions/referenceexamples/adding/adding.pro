TEMPLATE = app
TARGET = adding
DEPENDPATH += .
INCLUDEPATH += .
QT += declarative

# Input
SOURCES += main.cpp \
           person.cpp 
HEADERS += person.h
RESOURCES += adding.qrc
target.path = $$[QT_INSTALL_EXAMPLES]/declarative/cppextensions/referenceexamples/adding
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS adding.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/declarative/cppextensions/referenceexamples/adding
INSTALLS += target sources
