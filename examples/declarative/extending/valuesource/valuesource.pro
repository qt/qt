TEMPLATE = app
TARGET = valuesource
DEPENDPATH += .
INCLUDEPATH += .
QT += declarative

# Input
SOURCES += main.cpp \
           person.cpp \
           birthdayparty.cpp \
           happybirthdaysong.cpp 
HEADERS += person.h \
           birthdayparty.h \
           happybirthdaysong.h
RESOURCES += valuesource.qrc
target.path = $$[QT_INSTALL_EXAMPLES]/declarative/extending/valuesource
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS valuesource.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/declarative/extending/valuesource
INSTALLS += target sources
