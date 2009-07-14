TEMPLATE = app

SOURCES += main.cpp

TRANSLATIONS = project.ts
CONFIG += console

CODECFORSRC = CP1252
CODECFORTR = UTF-8

exists( $$TRANSLATIONS ) {
    win32: system(del $$TRANSLATIONS)
    unix:  system(rm $$TRANSLATIONS)
}


