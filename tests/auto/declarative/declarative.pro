TEMPLATE = subdirs
SUBDIRS += \
           graphicswidgets \        # Cover
           layouts \                # Cover
           parserstress \           # Cover
           qmetaobjectbuilder \     # Cover
           qmlanimations \          # Cover
           qmlbehaviors \           # Cover
           qmlbinding \             # Cover
           qmlconnection \          # Cover
           qmlcontext \             # Cover
           qmldatetimeformatter \   # Cover
           qmldebug \               # Cover
           qmldebugclient \         # Cover
           qmldebugservice \        # Cover
           qmldom \                 # Cover
           qmleasefollow \          # Cover
           qmlecmascript \          # Cover
           qmlengine     \          # Cover
           qmlerror \               # Cover
           qmlfontloader \          # Cover
           qmlgraphicsanchors \     # Cover
           qmlgraphicsanimatedimage \ # Cover
           qmlgraphicsimage \       # Cover
           qmlgraphicsborderimage \ # Cover
           qmlgraphicsflickable   \ # Cover
           qmlgraphicsflipable    \ # Cover
           qmlgraphicsgridview \    # Cover
           qmlgraphicsitem \        # Cover
           qmlgraphicslistview \    # Cover
           qmlgraphicsloader \      # Cover
           qmlgraphicsmouseregion \ # Cover
           qmlgraphicsparticles \   # Cover
           qmlgraphicspathview \    # Cover
           qmlgraphicspositioners \ # Cover
           qmlgraphicstext \        # Cover
           qmlgraphicstextedit \    # Cover
           qmlgraphicstextinput \   # Cover
           qmlinfo \                # Cover
           qmlinstruction \         # Cover
           qmllanguage \            # Cover
           qmllist \                # Cover
           qmllistaccessor \        # Cover
           qmllistmodel \           # Cover
           qmlmetaproperty \        # Cover
           qmlmetatype \            # Cover
           qmlnumberformatter \     # Cover
           qmlpixmapcache \         # Cover
           qmlpropertymap \         # Cover
           qmlqt \                  # Cover
           qmlspringfollow \        # Cover
           qmlstates \              # Cover
           qmlsystempalette \       # Cover
           qmltimer \               # Cover
           qmlxmllistmodel \        # Cover
           qpacketprotocol \        # Cover
           qmlgraphicsrepeater \    # Cover
           qmlvaluetypes \          # Cover
           qmlxmlhttprequest \      # Cover
           qmlimageprovider \       # Cover
           sql                      # Cover

contains(QT_CONFIG, webkit) {
    SUBDIRS += \
           qmlgraphicswebview       # Cover
}

# Tests which should run in Pulse
PULSE_TESTS = $$SUBDIRS

