TEMPLATE = app
LANGUAGE = Java

SOURCES += main.java

TRANSLATIONS        += project.ts

exists( $$TRANSLATIONS ) {
    win32: system(del $$TRANSLATIONS)
    unix:  system(rm $$TRANSLATIONS)
}

