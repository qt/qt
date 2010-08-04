include(../../../shared/fontpanel/fontpanel.pri)
TEMPLATE = app
LANGUAGE = C++
TARGET = assistant
contains(QT_CONFIG, webkit):QT += webkit
CONFIG += qt \
    warn_on \
    help
QT += network
PROJECTNAME = Assistant
DESTDIR = ../../../../bin
target.path = $$[QT_INSTALL_BINS]
INSTALLS += target
DEPENDPATH += ../shared
DEPENDPATH += .
INCLUDEPATH += .

# ## Work around a qmake issue when statically linking to
# ## not-yet-installed plugins
QMAKE_LIBDIR += $$QT_BUILD_TREE/plugins/sqldrivers
HEADERS += aboutdialog.h \
    bookmarkdialog.h \
    bookmarkfiltermodel.h \
    bookmarkitem.h \
    bookmarkmanager.h \
    bookmarkmanagerwidget.h \
    bookmarkmodel.h \
    centralwidget.h \
    cmdlineparser.h \
    contentwindow.h \
    findwidget.h \
    filternamedialog.h \
    helpenginewrapper.h \
    helpviewer.h \
    helpviewer_p.h \
    indexwindow.h \
    installdialog.h \
    mainwindow.h \
    preferencesdialog.h \
    qtdocinstaller.h \
    remotecontrol.h \
    searchwidget.h \
    topicchooser.h \
    tracer.h \
    xbelsupport.h \
    ../shared/collectionconfiguration.h \
    openpagesmodel.h \
    globalactions.h \
    openpageswidget.h \
    openpagesmanager.h \
    openpagesswitcher.h
win32:HEADERS += remotecontrol_win.h

SOURCES += aboutdialog.cpp \
    bookmarkdialog.cpp \
    bookmarkfiltermodel.cpp \
    bookmarkitem.cpp \
    bookmarkmanager.cpp \
    bookmarkmanagerwidget.cpp \
    bookmarkmodel.cpp \
    centralwidget.cpp \
    cmdlineparser.cpp \
    contentwindow.cpp \
    findwidget.cpp \
    filternamedialog.cpp \
    helpenginewrapper.cpp \
    helpviewer.cpp \
    indexwindow.cpp \
    installdialog.cpp \
    main.cpp \
    mainwindow.cpp \
    preferencesdialog.cpp \
    qtdocinstaller.cpp \
    remotecontrol.cpp \
    searchwidget.cpp \
    topicchooser.cpp \
    xbelsupport.cpp \
    ../shared/collectionconfiguration.cpp \
    openpagesmodel.cpp \
    globalactions.cpp \
    openpageswidget.cpp \
    openpagesmanager.cpp \
    openpagesswitcher.cpp
contains(QT_CONFIG, webkit) {
    SOURCES += helpviewer_qwv.cpp
} else {
    SOURCES += helpviewer_qtb.cpp
}

FORMS += bookmarkdialog.ui \
    bookmarkmanagerwidget.ui \
    bookmarkwidget.ui \
    filternamedialog.ui \
    installdialog.ui \
    preferencesdialog.ui \
    topicchooser.ui

RESOURCES += assistant.qrc \
    assistant_images.qrc

win32 {
    !wince*:LIBS += -lshell32
    RC_FILE = assistant.rc
}

mac {
    ICON = assistant.icns
    TARGET = Assistant
    QMAKE_INFO_PLIST = Info_mac.plist
}

contains(CONFIG, static): {
    SQLPLUGINS = $$unique(sql-plugins)
    contains(SQLPLUGINS, sqlite): {
        QTPLUGIN += qsqlite
        DEFINES += USE_STATIC_SQLITE_PLUGIN
    }
}
