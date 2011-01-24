INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/adaptationlayer.h \
    $$PWD/default/default_texturenode.h \
    $$PWD/default/default_rectanglenode.h \
    $$PWD/default/default_glyphnode.h \
    $$PWD/default/default_glyphnode_p.h \
#    $$PWD/threadedtexturemanager.h
    $$PWD/qsgpartialuploadtexturemanager.h \
    $$PWD/qsgthreadedtexturemanager.h \
    $$PWD/distancefield_glyphnode.h \
    $$PWD/distancefield_glyphnode_p.h \
    $$PWD/qvsyncanimationdriver_p.h \
    scenegraph/adaptationlayers/qsgmactexturemanager_mac_p.h

SOURCES += \
    $$PWD/adaptationlayer.cpp \
    $$PWD/default/default_texturenode.cpp \
    $$PWD/default/default_rectanglenode.cpp \
    $$PWD/default/default_glyphnode.cpp \
    $$PWD/default/default_glyphnode_p.cpp \
#    $$PWD/threadedtexturemanager.cpp
    $$PWD/qsgpartialuploadtexturemanager.cpp \
    $$PWD/qsgthreadedtexturemanager.cpp \
    $$PWD/distancefield_glyphnode.cpp \
    $$PWD/distancefield_glyphnode_p.cpp \
    $$PWD/qvsyncanimationdriver.cpp \
    scenegraph/adaptationlayers/qsgmactexturemanager_mac.cpp

#macx:{
#    SOURCES += $$PWD/mactexturemanager.cpp
#    HEADERS += $$PWD/mactexturemanager.h
#}


#contains(QT_CONFIG, qpa) {
#    SOURCES += $$PWD/qsgeglfsthreadedtexturemanager.cpp
#    HEADERS += $$PWD/qsgeglfsthreadedtexturemanager.h
#}
