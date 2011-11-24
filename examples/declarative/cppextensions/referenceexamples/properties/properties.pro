TEMPLATE = app
TARGET = properties
DEPENDPATH += .
INCLUDEPATH += .
QT += declarative

# Input
SOURCES += main.cpp \
           person.cpp \
           birthdayparty.cpp 
HEADERS += person.h \
           birthdayparty.h
RESOURCES += properties.qrc

target.path = $$[QT_INSTALL_EXAMPLES]/declarative/cppextensions/referenceexamples/properties
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS properties.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/declarative/cppextensions/referenceexamples/properties
INSTALLS += target sources
