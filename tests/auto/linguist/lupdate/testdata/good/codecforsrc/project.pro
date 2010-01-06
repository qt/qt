SOURCES += main.cpp
CONFIG+= console

TRANSLATIONS = project.ts

CODECFORTR = utf-8
CODECFORSRC = utf-8

exists( $$TRANSLATIONS ) {
    win32: system(del $$TRANSLATIONS)
    unix:  system(rm $$TRANSLATIONS)
}
