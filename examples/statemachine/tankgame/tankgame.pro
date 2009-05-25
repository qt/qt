HEADERS += mainwindow.h \
           plugin.h \
           tankitem.h \
           rocketitem.h \
           gameitem.h \
           gameovertransition.h
SOURCES += main.cpp \
           mainwindow.cpp \
           tankitem.cpp \
           rocketitem.cpp \
           gameitem.cpp \
           gameovertransition.cpp
CONFIG += console

# install
target.path = $$[QT_INSTALL_EXAMPLES]/statemachine/tankgame
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS tankgame.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/statemachine/tankgame
INSTALLS += target sources
