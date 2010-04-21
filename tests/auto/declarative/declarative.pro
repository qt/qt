TEMPLATE = subdirs
SUBDIRS += \
           examples \
           graphicswidgets \        # Cover
           parserstress \           # Cover
           qmetaobjectbuilder \     # Cover
           qdeclarativeanimations \          # Cover
           qdeclarativebehaviors \           # Cover
           qdeclarativebinding \             # Cover
           qdeclarativecomponent \           # Cover
           qdeclarativeconnection \          # Cover
           qdeclarativecontext \             # Cover
           qdeclarativedebug \               # Cover
           qdeclarativedebugclient \         # Cover
           qdeclarativedebugservice \        # Cover
           qdeclarativedom \                 # Cover
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
           qdeclarativefocusscope  \ # Cover
           qdeclarativegridview \    # Cover
           qdeclarativeitem \        # Cover
           qdeclarativelistview \    # Cover
           qdeclarativeloader \      # Cover
           qdeclarativelayouts \     # Cover
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
           qdeclarativeproperty \            # Cover
           qdeclarativemetatype \            # Cover
           qdeclarativemoduleplugin \        # Cover
           qdeclarativepixmapcache \         # Cover
           qdeclarativepropertymap \         # Cover
           qdeclarativeqt \                  # Cover
           qdeclarativesmoothedanimation \   # Cover
           qdeclarativesmoothedfollow\       # Cover
           qdeclarativespringfollow \        # Cover
           qdeclarativestates \              # Cover
           qdeclarativesystempalette \       # Cover
           qdeclarativetimer \               # Cover
           qdeclarativexmllistmodel \        # Cover
           qpacketprotocol \        # Cover
           qdeclarativerepeater \    # Cover
           qdeclarativeworkerscript \    # Cover
           qdeclarativevaluetypes \          # Cover
           qdeclarativexmlhttprequest \      # Cover
           qdeclarativeimageprovider \       # Cover
           qdeclarativestyledtext \          # Cover
           qdeclarativesqldatabase \         # Cover
           qmlvisual                    # Cover

contains(QT_CONFIG, webkit) {
    SUBDIRS += \
           qdeclarativewebview       # Cover
}

# Tests which should run in Pulse
PULSE_TESTS = $$SUBDIRS

