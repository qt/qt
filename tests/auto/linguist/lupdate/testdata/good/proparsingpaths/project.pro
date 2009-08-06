SOURCES += file*.cpp filter.cpp non-existing.cpp

include(sub/sub.pri)

TRANSLATIONS = project.ts

exists( $$TRANSLATIONS ) {
    win32: system(del $$TRANSLATIONS)
    unix:  system(rm -f $$TRANSLATIONS)
}
