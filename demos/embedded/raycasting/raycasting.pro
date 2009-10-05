TEMPLATE = app
SOURCES = raycasting.cpp
RESOURCES += raycasting.qrc

symbian {
    TARGET.UID3 = 0xA000CF76
    include($$QT_SOURCE_TREE/demos/symbianpkgrules.pri)
}
