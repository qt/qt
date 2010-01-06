SOURCES += main.cpp
CONFIG += console

TRANSLATIONS = project.ts

CODECFORSRC = CP1252
CODECFORTR = UTF-8

exists( $$TRANSLATIONS ) {
    win32: system(del $$TRANSLATIONS)
    unix:  system(rm $$TRANSLATIONS)
}


