# Include those manually as they do not contain any directory specification

FORMS    = ../config.ui
HEADERS  = ../qvfb.h \
           ../qvfbview.h \
           ../qvfbratedlg.h \
           ../qanimationwriter.h \
           ../gammaview.h \
           ../qvfbprotocol.h \
           ../qvfbshmem.h \
           ../qvfbmmap.h \
           ../../../src/gui/embedded/qvfbhdr.h \
           ../../../src/gui/embedded/qlock_p.h \
           ../../../src/gui/embedded/qwssignalhandler_p.h \
           ../../shared/deviceskin/deviceskin.h

SOURCES =  ../qvfb.cpp \
           ../qvfbview.cpp \
           ../qvfbratedlg.cpp \
           ../main.cpp \
           ../qanimationwriter.cpp \
           ../qvfbprotocol.cpp \
           ../qvfbshmem.cpp \
           ../qvfbmmap.cpp \
           ../../../src/gui/embedded/qlock.cpp \
           ../../../src/gui/embedded/qwssignalhandler.cpp \
           ../../shared/deviceskin/deviceskin.cpp

TRANSLATIONS=$$[QT_INSTALL_TRANSLATIONS]/qvfb_pl.ts \
             $$[QT_INSTALL_TRANSLATIONS]/qvfb_ru.ts \
             $$[QT_INSTALL_TRANSLATIONS]/qvfb_zh_CN.ts \
             $$[QT_INSTALL_TRANSLATIONS]/qvfb_zh_TW.ts
