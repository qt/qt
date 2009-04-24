SOURCES = main.cpp
CONFIG -= qt app_bundle
CONFIG += console

!win32-g++:win32:!equals(TEMPLATE_PREFIX, "vc"):QMAKE_CXXFLAGS += /GS-
DESTDIR = ./



