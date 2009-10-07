# Include those manually as they do not contain any directory specification

FORMS += ../compat/helpdialog.ui \
         ../compat/mainwindow.ui \
         ../compat/tabbedbrowser.ui \
         ../compat/topicchooser.ui

SOURCES += ../compat/main.cpp \
           ../compat/helpwindow.cpp \
           ../compat/topicchooser.cpp \
           ../compat/docuparser.cpp \
           ../compat/index.cpp \
           ../compat/profile.cpp \
           ../compat/config.cpp \
           ../compat/helpdialog.cpp \
           ../compat/mainwindow.cpp \
           ../compat/tabbedbrowser.cpp \
           ../compat/fontsettingsdialog.cpp

SOURCES += ../../shared/fontpanel/fontpanel.cpp

HEADERS += ../compat/helpwindow.h \
           ../compat/topicchooser.h \
           ../compat/docuparser.h \
           ../compat/index.h \
           ../compat/profile.h \
           ../compat/helpdialog.h \
           ../compat/mainwindow.h \
           ../compat/tabbedbrowser.h \
           ../compat/config.h \
           ../compat/fontsettingsdialog.h


TR_DIR = $$PWD/../../../translations
TRANSLATIONS = \
    $$TR_DIR/assistant_adp_de.ts \
    $$TR_DIR/assistant_adp_ja.ts \
    $$TR_DIR/assistant_adp_pl.ts \
    $$TR_DIR/assistant_adp_ru.ts \
    $$TR_DIR/assistant_adp_zh_CN.ts \
    $$TR_DIR/assistant_adp_zh_TW.ts
