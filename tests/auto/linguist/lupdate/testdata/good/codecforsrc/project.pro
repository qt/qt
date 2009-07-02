TEMPLATE = app
TARGET +=
DEPENDPATH += .
INCLUDEPATH += .

# Input
SOURCES += main.cpp

TRANSLATIONS = project.ts
CONFIG+= console

CODECFORTR = utf-8
CODECFORSRC = utf-8

exists( $$TRANSLATIONS ) {
    win32: system(del $$TRANSLATIONS)
    unix:  system(rm $$TRANSLATIONS)
}


