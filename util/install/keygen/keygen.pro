CONFIG += qt console
SOURCES += keyinfo.cpp
keychk {
  TEMPLATE = lib
  TARGET = keychk
  DESTDIR = ../../../lib/
  CONFIG -= shared dll
  CONFIG += staticlib
} else {
  TEMPLATE = app
  TARGET = keygen
  SOURCES += main.cpp
}