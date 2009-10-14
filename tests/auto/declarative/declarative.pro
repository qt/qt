TEMPLATE = subdirs
SUBDIRS += anchors \
           animations \
           datetimeformatter \
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

