TEMPLATE = subdirs
!symbian: {
SUBDIRS += \
           qdeclarativemetatype \
           qmetaobjectbuilder
}

SUBDIRS += \
           examples \
           parserstress \
           qdeclarativeanchors \
           qdeclarativeanimatedimage \
           qdeclarativeanimations \
           qdeclarativebehaviors \
           qdeclarativebinding \
           qdeclarativeborderimage \
           qdeclarativecomponent \
           qdeclarativeconnection \
           qdeclarativecontext \
           qdeclarativedebug \
           qdeclarativedebugclient \
           qdeclarativedebugservice \
           qdeclarativedom \
           qdeclarativeecmascript \
           qdeclarativeengine \
           qdeclarativeerror \
           qdeclarativefolderlistmodel \
           qdeclarativefontloader \
           qdeclarativeflickable \
           qdeclarativeflipable \
           qdeclarativefocusscope \
           qdeclarativegridview \
           qdeclarativeimage \
           qdeclarativeimageprovider \
           qdeclarativeinfo \
           qdeclarativeinstruction \
           qdeclarativeitem \
           qdeclarativelanguage \
           qdeclarativelayoutitem \
           qdeclarativelistmodel \
           qdeclarativelistreference \
           qdeclarativelistview \
           qdeclarativeloader \
           qdeclarativemoduleplugin \
           qdeclarativemousearea \
           qdeclarativeparticles \
           qdeclarativepathview \
           qdeclarativepixmapcache \
           qdeclarativepositioners \
           qdeclarativeproperty \
           qdeclarativepropertymap \
           qdeclarativeqt \
           qdeclarativerepeater \
           qdeclarativesmoothedanimation \
           qdeclarativespringanimation \
           qdeclarativesqldatabase \
           qdeclarativestates \
           qdeclarativestyledtext \
           qdeclarativesystempalette \
           qdeclarativetext \
           qdeclarativetextedit \
           qdeclarativetextinput \
           qdeclarativetimer \
           qdeclarativevaluetypes \
           qdeclarativeview \
           qdeclarativeviewer \
           qdeclarativevisualdatamodel \
           qdeclarativeworkerscript \
           qdeclarativexmlhttprequest \
           qdeclarativexmllistmodel \
           qmlvisual \
           qpacketprotocol \
           moduleqt47

contains(QT_CONFIG, webkit) {
    SUBDIRS += \
           qdeclarativewebview
}

# Tests which should run in Pulse
PULSE_TESTS = $$SUBDIRS
