TEMPLATE = app
SOURCES = raycasting.cpp
RESOURCES += raycasting.qrc

symbian {
    include($$QT_SOURCE_TREE/demos/symbianpkgrules.pri)
    TARGET.UID3 = 0xA000CF76
}
