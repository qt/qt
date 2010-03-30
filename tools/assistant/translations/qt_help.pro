# Include those manually as they do not contain any directory specification

SOURCES += ../lib/qhelpcollectionhandler.cpp \
           ../lib/qhelpcontentwidget.cpp \
           ../lib/qhelpdatainterface.cpp \
           ../lib/qhelpdbreader.cpp \
           ../lib/qhelpengine.cpp \
           ../lib/qhelpenginecore.cpp \
           ../lib/qhelpgenerator.cpp \
           ../lib/qhelpindexwidget.cpp \
           ../lib/qhelpprojectdata.cpp \
           ../lib/qhelpsearchengine.cpp \
           ../lib/qhelpsearchindexreader_clucene.cpp \
           ../lib/qhelpsearchindexreader_default.cpp \
           ../lib/qhelpsearchindexwriter_clucene.cpp \
           ../lib/qhelpsearchindexwriter_default.cpp \
           ../lib/qhelpsearchindex_default.cpp \
           ../lib/qhelpsearchquerywidget.cpp \
           ../lib/qhelpsearchresultwidget.cpp

HEADERS += ../lib/qhelpcollectionhandler_p.h \
           ../lib/qhelpcontentwidget.h \
           ../lib/qhelpdatainterface_p.h \
           ../lib/qhelpdbreader_p.h \
           ../lib/qhelpengine.h \
           ../lib/qhelpenginecore.h \
           ../lib/qhelpengine_p.h \
           ../lib/qhelpgenerator_p.h \
           ../lib/qhelpindexwidget.h \
           ../lib/qhelpprojectdata_p.h \
           ../lib/qhelpsearchengine.h \
           ../lib/qhelpsearchindexreader_clucene_p.h \
           ../lib/qhelpsearchindexreader_default_p.h \
           ../lib/qhelpsearchindexwriter_clucene_p.h \
           ../lib/qhelpsearchindexwriter_default_p.h \
           ../lib/qhelpsearchindex_default_p.h \
           ../lib/qhelpsearchquerywidget.h \
           ../lib/qhelpsearchresultwidget.h \
           ../lib/qhelp_global.h


TR_DIR = $$PWD/../../../translations
TRANSLATIONS = \
    $$TR_DIR/qt_help_cs.ts \
    $$TR_DIR/qt_help_da.ts \
    $$TR_DIR/qt_help_de.ts \
    $$TR_DIR/qt_help_hu.ts \
    $$TR_DIR/qt_help_ja.ts \
    $$TR_DIR/qt_help_pl.ts \
    $$TR_DIR/qt_help_ru.ts \
    $$TR_DIR/qt_help_zh_CN.ts \
    $$TR_DIR/qt_help_zh_TW.ts \
    $$TR_DIR/qt_help_fr.ts
