TEMPLATE = app
LANGUAGE = C++

include(win/win.pri)
include(mac/mac.pri)
include(unix/unix.pri)
include (common/common.pri)             # Important: keep the space before the '('
include(relativity/relativity.pri)

message($$SOURCES)
TRANSLATIONS = project.ts

exists( $$TRANSLATIONS ) {
    win32: system(del $$TRANSLATIONS)
    unix:  system(rm $$TRANSLATIONS)
}
