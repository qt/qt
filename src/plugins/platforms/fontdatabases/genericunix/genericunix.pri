contains(QT_CONFIG, fontconfig) {
    include(../fontconfig/fontconfig.pri)
    DEFINES += Q_FONTCONFIGDATABASE
} else {
    include(../basicunix/basicunix.pri)
}

INCLUDEPATH += $$QT_SOURCE_TREE/src/plugins/platforms/fontdatabases/genericunix
HEADERS += \
           $$QT_SOURCE_TREE/src/plugins/platforms/fontdatabases/genericunix/qgenericunixfontdatabase.h
