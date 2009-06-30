TEMPLATE = app
LANGUAGE = C++

SOURCES += main.cpp
SOURCES += finddialog.cpp

TRANSLATIONS        += project.ts

exists( $$TRANSLATIONS ) {
    win32: system(del $$TRANSLATIONS)
    unix:  system(rm $$TRANSLATIONS)
}

