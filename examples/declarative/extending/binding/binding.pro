TEMPLATE = app
TARGET = binding
DEPENDPATH += .
INCLUDEPATH += .
QT += declarative

# Input
SOURCES += main.cpp \
           person.cpp \
           birthdayparty.cpp \
           happybirthday.cpp 
HEADERS += person.h \
           birthdayparty.h \
           happybirthday.h
RESOURCES += binding.qrc
target.path = $$[QT_INSTALL_EXAMPLES]/declarative/extending/binding
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS binding.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/declarative/extending/binding
INSTALLS += target sources
