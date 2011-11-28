TEMPLATE = app
TARGET = coercion
DEPENDPATH += .
INCLUDEPATH += .
QT += declarative

# Input
SOURCES += main.cpp \
           person.cpp \
           birthdayparty.cpp 
HEADERS += person.h \
           birthdayparty.h
RESOURCES += coercion.qrc
target.path = $$[QT_INSTALL_EXAMPLES]/declarative/cppextensions/referenceexamples/coercion
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS coercion.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/declarative/cppextensions/referenceexamples/coercion
INSTALLS += target sources
