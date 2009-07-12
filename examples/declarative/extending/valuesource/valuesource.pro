TEMPLATE = app
TARGET = valuesource
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
RESOURCES += valuesource.qrc
