SOURCES = wayland.cpp
CONFIG -= qt

QMAKE_CXXFLAGS += $$QT_CFLAGS_WAYLAND
LIBS += $$QT_LIBS_WAYLAND -lfreetype -lfontconfig -lwayland-client

