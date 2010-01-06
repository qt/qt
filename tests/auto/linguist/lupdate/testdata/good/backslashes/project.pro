SOURCES += src\main.cpp

TRANSLATIONS = ts\project.ts


!exists(ts) {
    win32: system(md ts)
    else: system(mkdir ts)
}
