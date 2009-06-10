
INCLUDEPATH += $$PWD

HEADERS += $$PWD/javascriptast_p.h \
           $$PWD/javascriptastfwd_p.h \
           $$PWD/javascriptastvisitor_p.h \
           $$PWD/javascriptengine_p.h \
           $$PWD/javascriptgrammar_p.h \
           $$PWD/javascriptlexer_p.h \
           $$PWD/javascriptmemorypool_p.h \
           $$PWD/javascriptnodepool_p.h \
           $$PWD/javascriptparser_p.h \
           $$PWD/javascriptprettypretty_p.h

SOURCES += $$PWD/javascriptast.cpp \
           $$PWD/javascriptastvisitor.cpp \
           $$PWD/javascriptengine_p.cpp \
           $$PWD/javascriptgrammar.cpp \
           $$PWD/javascriptlexer.cpp \
           $$PWD/javascriptprettypretty.cpp \
           $$PWD/javascriptparser.cpp

