TEMPLATE = lib
CONFIG += dll
CONFIG -= staticlib
SOURCES		= mylib.c
TARGET = mylib
DESTDIR = ../
QT = core

wince*: DEFINES += WIN32_MSVC
win32-msvc: DEFINES += WIN32_MSVC
win32-borland: DEFINES += WIN32_BORLAND

# Force a copy of the library to have an extension that is non-standard.
# We want to test if we can load a shared library with *any* filename...

# For windows test if we can load a filename with multiple dots.
win32: {
    cp = $(COPY) $(DESTDIR_TARGET) ..$$QMAKE_DIR_SEP
    QMAKE_POST_LINK = \
        $${cp}mylib.dl2 && \
        $${cp}system.trolltech.test.mylib.dll && \
        $${cp}mylib_noextension
}
unix:!symbian: {
    cp = $(COPY) $(DESTDIR)$(TARGET) ../
    QMAKE_POST_LINK = \
        $${cp}libmylib.so2 && \
        $${cp}system.trolltech.test.mylib.so
}

#no special install rule for the library used by test
INSTALLS =

symbian: TARGET.CAPABILITY=ALL -TCB

