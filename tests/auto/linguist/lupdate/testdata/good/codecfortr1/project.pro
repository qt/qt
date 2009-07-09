TEMPLATE = app

SOURCES += main.cpp

TRANSLATIONS = project.ts
CONFIG += console

CODECFORTR = CP1252

exists( $$TRANSLATIONS ) {
    win32: system(del $$TRANSLATIONS)
    unix:  system(rm $$TRANSLATIONS)
}


