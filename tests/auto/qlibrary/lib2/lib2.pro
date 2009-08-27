TEMPLATE = lib
CONFIG += dll
CONFIG -= staticlib
SOURCES		= mylib.c
TARGET = mylib
DESTDIR = ../
VERSION = 2
QT = core

wince*: DEFINES += WIN32_MSVC
win32-msvc: DEFINES += WIN32_MSVC
win32-borland: DEFINES += WIN32_BORLAND

# Force a copy of the library to have an extension that is non-standard.
# We want to test if we can load a shared library with *any* filename...

# For windows test if we can load a filename with multiple dots.
win32: {
    QMAKE_POST_LINK = copy /Y ..\mylib2.dll ..\mylib.dl2 && \
    copy /Y ..\mylib2.dll ..\system.trolltech.test.mylib.dll
}

unix:!symbian: {
    QMAKE_POST_LINK = cp -f $(DESTDIR)$(TARGET) ../libmylib.so2 && \
    cp -f $(DESTDIR)$(TARGET) ../system.trolltech.test.mylib.so
}

symbian-abld: {
    TARGET.CAPABILITY=ALL -TCB
    FIXEDROOT = $$replace(EPOCROOT,/,\\)
	QMAKE_POST_LINK = \
    copy /Y $${FIXEDROOT}epoc32\release\\$(PLATFORM)\\$(CFG)\mylib.dll  $${FIXEDROOT}epoc32\release\\$(PLATFORM)\\$(CFG)\mylib.dl2 && \
    copy /Y $${FIXEDROOT}epoc32\release\\$(PLATFORM)\\$(CFG)\mylib.dll  $${FIXEDROOT}epoc32\release\\$(PLATFORM)\\$(CFG)\system.trolltech.test.mylib.dll && \
    IF NOT "$(PLATFORM)==WINSCW" copy /Y $${FIXEDROOT}epoc32\release\\$(PLATFORM)\\$(CFG)\mylib.dll  ..\tst\mylib.dl2
}

symbian-sbsv2: {
    TARGET.CAPABILITY=ALL -TCB
    QMAKE_POST_LINK = \
    $(GNUCP) $${EPOCROOT}epoc32/release/$(PLATFORM_PATH)/$(CFG_PATH)/mylib.dll  $${EPOCROOT}epoc32/release/$(PLATFORM_PATH)/$(CFG_PATH)/mylib.dl2 && \
    $(GNUCP) $${EPOCROOT}epoc32/release/$(PLATFORM_PATH)/$(CFG_PATH)/mylib.dll  $${EPOCROOT}epoc32/release/$(PLATFORM_PATH)/$(CFG_PATH)/system.trolltech.test.mylib.dll && \
    if test $(PLATFORM) != WINSCW;then $(GNUCP) $${EPOCROOT}epoc32/release/$(PLATFORM_PATH)/$(CFG_PATH)/mylib.dll $${PWD}/../tst/mylib.dl2; fi
}

#no special install rule for the library used by test
INSTALLS =


