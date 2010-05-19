TEMPLATE = app
TARGET = default
DEPENDPATH += .
INCLUDEPATH += .
QT += declarative

# Input
SOURCES += main.cpp \
           person.cpp \
           birthdayparty.cpp 
HEADERS += person.h \
           birthdayparty.h
RESOURCES += default.qrc
target.path = $$[QT_INSTALL_EXAMPLES]/declarative/extending/default
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS default.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/declarative/extending/default
INSTALLS += target sources
