SOURCES = flickable.cpp main.cpp
HEADERS = flickable.h

symbian {
    TARGET.UID3 = 0xA000CF73
    include($$QT_SOURCE_TREE/demos/symbianpkgrules.pri)
}
