INCLUDEPATH += $$PWD
LIBS += -lmediaclientvideo \
	-lmediaclientaudio \
	-lws32 \
	-lfbscli \
	-lcone \
    	-lmmfcontrollerframework \
    	-lefsrv \
    	-lbitgdi \
    	-lapgrfx \
    	-lapmime


# We are building Symbian backend with media player support
DEFINES += QMEDIA_MMF_PLAYER


HEADERS += \
    $$PWD/s60mediaplayercontrol.h \
    $$PWD/s60mediaplayerservice.h \
    $$PWD/s60mediaplayersession.h \
    $$PWD/s60videoplayersession.h \
    $$PWD/s60mediametadataprovider.h \
    $$PWD/s60videosurface.h \
    $$PWD/s60videooverlay.h \
    $$PWD/s60videorenderer.h \
    $$PWD/s60mediarecognizer.h \
    $$PWD/s60audioplayersession.h \
    $$PWD/ms60mediaplayerresolver.h \
    $$PWD/s60videowidget.h \
    $$PWD/s60mediaplayeraudioendpointselector.h

SOURCES += \
    $$PWD/s60mediaplayercontrol.cpp \
    $$PWD/s60mediaplayerservice.cpp \
    $$PWD/s60mediaplayersession.cpp \
    $$PWD/s60videoplayersession.cpp \
    $$PWD/s60mediametadataprovider.cpp \
    $$PWD/s60videosurface.cpp \
    $$PWD/s60videooverlay.cpp \
    $$PWD/s60videorenderer.cpp \
    $$PWD/s60mediarecognizer.cpp \
    $$PWD/s60audioplayersession.cpp \
    $$PWD/s60videowidget.cpp \
    $$PWD/s60mediaplayeraudioendpointselector.cpp

contains(S60_VERSION, 3.1) {

    #3.1 doesn't provide audio routing in videoplayer
    DEFINES += HAS_NO_AUDIOROUTING_IN_VIDEOPLAYER

    !exists($${EPOCROOT}epoc32\release\winscw\udeb\audiooutputrouting.lib) {
        MMP_RULES += "$${LITERAL_HASH}ifdef WINSCW" \
                     "MACRO HAS_NO_AUDIOROUTING" \
                     "$${LITERAL_HASH}else" \
                     "LIBRARY audiooutputrouting.lib" \
                     "$${LITERAL_HASH}endif"
    }

} else {
    LIBS += -laudiooutputrouting
}

