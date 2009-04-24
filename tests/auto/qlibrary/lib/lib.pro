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
    QMAKE_POST_LINK = copy /Y $(DESTDIR_TARGET) ..\mylib.dl2 && \
    copy /Y $(DESTDIR_TARGET) ..\system.trolltech.test.mylib.dll && \
    copy /Y $(DESTDIR_TARGET) ..\mylib_noextension
}
unix:!symbian: {
    QMAKE_POST_LINK = cp -f $(DESTDIR)$(TARGET) ../libmylib.so2 && \
    cp -f $(DESTDIR)$(TARGET) ../system.trolltech.test.mylib.so
}

#no special install rule for the library used by test
INSTALLS =

symbian: TARGET.CAPABILITY=ALL -TCB

