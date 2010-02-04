# Include those manually as they do not contain any directory specification

FORMS += ../tools/assistant/filternamedialog.ui \
         ../tools/assistant/installdialog.ui \
         ../tools/assistant/preferencesdialog.ui \
         ../tools/assistant/topicchooser.ui \
         ../tools/assistant/bookmarkdialog.ui

SOURCES += ../tools/assistant/aboutdialog.cpp \
           ../tools/assistant/bookmarkmanager.cpp \
           ../tools/assistant/centralwidget.cpp \
           ../tools/assistant/cmdlineparser.cpp \
           ../tools/assistant/contentwindow.cpp \
           ../tools/assistant/filternamedialog.cpp \
           ../tools/assistant/helpviewer.cpp \
           ../tools/assistant/indexwindow.cpp \
           ../tools/assistant/installdialog.cpp \
           ../tools/assistant/main.cpp \
           ../tools/assistant/mainwindow.cpp \
           ../tools/assistant/preferencesdialog.cpp \
           ../tools/assistant/remotecontrol.cpp \
           ../tools/assistant/searchwidget.cpp \
           ../tools/assistant/topicchooser.cpp \

SOURCES += ../../shared/fontpanel/fontpanel.cpp

HEADERS += ../tools/assistant/aboutdialog.h \
           ../tools/assistant/bookmarkmanager.h \
           ../tools/assistant/centralwidget.h \
           ../tools/assistant/cmdlineparser.h \
           ../tools/assistant/contentwindow.h \
           ../tools/assistant/filternamedialog.h \
           ../tools/assistant/helpviewer.h \
           ../tools/assistant/indexwindow.h \
           ../tools/assistant/installdialog.h \
           ../tools/assistant/mainwindow.h \
           ../tools/assistant/preferencesdialog.h \
           ../tools/assistant/remotecontrol.h \
           ../tools/assistant/remotecontrol_win.h \
           ../tools/assistant/searchwidget.h \
           ../tools/assistant/topicchooser.h \

TR_DIR = $$PWD/../../../translations
TRANSLATIONS = \
    $$TR_DIR/assistant_cs.ts \
    $$TR_DIR/assistant_da.ts \
    $$TR_DIR/assistant_de.ts \
    $$TR_DIR/assistant_fr.ts \
    $$TR_DIR/assistant_ja.ts \
    $$TR_DIR/assistant_pl.ts \
    $$TR_DIR/assistant_ru.ts \
    $$TR_DIR/assistant_zh_CN.ts \
    $$TR_DIR/assistant_zh_TW.ts
