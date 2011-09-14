
INCLUDEPATH *= $$PWD

DEFINES += PROEVALUATOR_CUMULATIVE PROEVALUATOR_INIT_PROPS

HEADERS += \
    $$PWD/proparser_global.h \
    $$PWD/ioutils.h \
    $$PWD/proitems.h \
    $$PWD/profileparser.h \
    $$PWD/profileevaluator.h

SOURCES += \
    $$PWD/ioutils.cpp \
    $$PWD/proitems.cpp \
    $$PWD/profileparser.cpp \
    $$PWD/profileevaluator.cpp
