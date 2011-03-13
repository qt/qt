INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/adaptationlayer.h \
    $$PWD/default/default_texturenode.h \
    $$PWD/default/default_rectanglenode.h \
    $$PWD/default/default_glyphnode.h \
    $$PWD/default/default_glyphnode_p.h \
    $$PWD/distancefield_glyphnode.h \
    $$PWD/distancefield_glyphnode_p.h

SOURCES += \
    $$PWD/adaptationlayer.cpp \
    $$PWD/default/default_texturenode.cpp \
    $$PWD/default/default_rectanglenode.cpp \
    $$PWD/default/default_glyphnode.cpp \
    $$PWD/default/default_glyphnode_p.cpp \
    $$PWD/distancefield_glyphnode.cpp \
    $$PWD/distancefield_glyphnode_p.cpp

macx:{
    SOURCES += $$PWD/qsgmactexturemanager_mac.cpp
    HEADERS += $$PWD/qsgmactexturemanager_mac_p.h
}


#contains(QT_CONFIG, qpa) {
#    SOURCES += $$PWD/qsgeglfsthreadedtexturemanager.cpp
#    HEADERS += $$PWD/qsgeglfsthreadedtexturemanager.h
#}
