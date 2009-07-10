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
