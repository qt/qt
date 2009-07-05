TEMPLATE = app
TARGET +=
DEPENDPATH += .
INCLUDEPATH += .

# Input
SOURCES += main.cpp

TRANSLATIONS = project.ts
CONFIG+= console

CODECFORTR = CP1251

exists( $$TRANSLATIONS ) {
    win32: system(del $$TRANSLATIONS)
    unix:  system(rm $$TRANSLATIONS)
}


