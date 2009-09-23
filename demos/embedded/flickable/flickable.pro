SOURCES = flickable.cpp main.cpp
HEADERS = flickable.h

symbian {
    include($$QT_SOURCE_TREE/demos/symbianpkgrules.pri)
    TARGET.UID3 = 0xA000CF73
}
