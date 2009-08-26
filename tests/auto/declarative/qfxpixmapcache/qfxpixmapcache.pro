load(qttest_p4)
contains(QT_CONFIG,declarative): QT += declarative
QT += network
SOURCES += tst_qfxpixmapcache.cpp
HEADERS = 
macx:CONFIG -= app_bundle

# QMAKE_CXXFLAGS = -fprofile-arcs -ftest-coverage
# LIBS += -lgcov
