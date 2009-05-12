TEMPLATE = app
TARGET = softkeys

HEADERS += \
    qsoftkeystack.h \
    qsoftkeystack_p.h \
    qsoftkeyaction.h

SOURCES += \
    main.cpp \
    qsoftkeyaction.cpp \
    qsoftkeystack_s60.cpp \
    qsoftkeystack.cpp

iCONFIG += qt debug warn_on
