load(qttest_p4)
contains(QT_CONFIG,declarative): QT += declarative
SOURCES += tst_qmllanguage.cpp \
           testtypes.cpp
HEADERS += testtypes.h
macx:CONFIG -= app_bundle

# QMAKE_CXXFLAGS = -fprofile-arcs -ftest-coverage
# LIBS += -lgcov
