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

SUBDIRS -= examples # Human-interactive

# Tests which should run in Pulse
PULSE_TESTS = $$SUBDIRS
PULSE_TESTS -= visual # All except 'visual' tests, allegedly too flaky

