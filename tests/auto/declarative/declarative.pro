TEMPLATE = subdirs
SUBDIRS += datetimeformatter \
           numberformatter \
           qbindablemap \
           layouts \
           listview \
           pathview \
           qfxtext \
           qfxtextedit \
           simplecanvasitem \
           repeater \
           qmllanguage \
           qmlecmascript \
           qmlmetaproperty \
           qmllist \
           qmllistaccessor \
           visual

# Tests which should run in Pulse
PULSE_TESTS = $$SUBDIRS
PULSE_TESTS -= visual # All except 'visual' tests, allegedly too flaky

