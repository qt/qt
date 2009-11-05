load(qttest_p4)
contains(QT_CONFIG,declarative): QT += declarative
QT += script
macx:CONFIG -= app_bundle

SOURCES += tst_qmllanguage.cpp \
           testtypes.cpp
HEADERS += testtypes.h

# QMAKE_CXXFLAGS = -fprofile-arcs -ftest-coverage
# LIBS += -lgcov
