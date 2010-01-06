SOURCES += main.cpp
CONFIG+= console

TRANSLATIONS = project.ts

CODECFORTR = CP1251

exists( $$TRANSLATIONS ) {
    win32: system(del $$TRANSLATIONS)
    unix:  system(rm $$TRANSLATIONS)
}


