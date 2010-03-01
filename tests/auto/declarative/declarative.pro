TEMPLATE = subdirs
SUBDIRS += \
           graphicswidgets \        # Cover
           layouts \                # Cover
           parserstress \           # Cover
           qmetaobjectbuilder \     # Cover
           qdeclarativeanimations \          # Cover
           qdeclarativebehaviors \           # Cover
           qdeclarativebinding \             # Cover
           qdeclarativeconnection \          # Cover
           qdeclarativecontext \             # Cover
           qdeclarativedatetimeformatter \   # Cover
           qdeclarativedebug \               # Cover
           qdeclarativedebugclient \         # Cover
           qdeclarativedebugservice \        # Cover
           qdeclarativedom \                 # Cover
           qdeclarativeeasefollow \          # Cover
           qdeclarativeecmascript \          # Cover
           qdeclarativeengine     \          # Cover
           qdeclarativeerror \               # Cover
           qdeclarativefontloader \          # Cover
           qdeclarativeanchors \     # Cover
           qdeclarativeanimatedimage \ # Cover
           qdeclarativeimage \       # Cover
           qdeclarativeborderimage \ # Cover
           qdeclarativeflickable   \ # Cover
           qdeclarativeflipable    \ # Cover
           qdeclarativegridview \    # Cover
           qdeclarativeitem \        # Cover
           qdeclarativelistview \    # Cover
           qdeclarativeloader \      # Cover
           qdeclarativemousearea \   # Cover
           qdeclarativeparticles \   # Cover
           qdeclarativepathview \    # Cover
           qdeclarativepositioners \ # Cover
           qdeclarativetext \        # Cover
           qdeclarativetextedit \    # Cover
           qdeclarativetextinput \   # Cover
           qdeclarativeinfo \                # Cover
           qdeclarativeinstruction \         # Cover
           qdeclarativelanguage \            # Cover
           qdeclarativelistreference \       # Cover
           qdeclarativelistmodel \           # Cover
           qdeclarativeproperty \        # Cover
           qdeclarativemetatype \            # Cover
           qdeclarativemoduleplugin \        # Cover
           qdeclarativenumberformatter \     # Cover
           qdeclarativepixmapcache \         # Cover
           qdeclarativepropertymap \         # Cover
           qdeclarativeqt \                  # Cover
           qdeclarativespringfollow \        # Cover
           qdeclarativestates \              # Cover
           qdeclarativesystempalette \       # Cover
           qdeclarativetimer \               # Cover
           qdeclarativexmllistmodel \        # Cover
           qpacketprotocol \        # Cover
           qdeclarativerepeater \    # Cover
           qdeclarativevaluetypes \          # Cover
           qdeclarativexmlhttprequest \      # Cover
           qdeclarativeimageprovider \       # Cover
           qdeclarativestyledtext \          # Cover
           sql                      # Cover

contains(QT_CONFIG, webkit) {
    SUBDIRS += \
           qdeclarativewebview       # Cover
}

# Tests which should run in Pulse
PULSE_TESTS = $$SUBDIRS

