TEMPLATE = subdirs
SUBDIRS += anchors \
           animatedimage \
           animations \
           behaviors \
           datetimeformatter \
           examples \
           layouts \
           listview \
           numberformatter \
           pathview \
           qbindablemap \
           qfxloader \
           qfxpixmapcache \
           qfxtext \
           qfxtextedit \
           qfxtextinput \
           qfxwebview \
           qmldom \
           qmlecmascript \
           qmllanguage \
           qmllist \
           qmllistaccessor \
           qmlmetaproperty \
           qmltimer \
           repeater \
           sql \
           states \
           visual

# Tests which should run in Pulse
PULSE_TESTS = $$SUBDIRS

