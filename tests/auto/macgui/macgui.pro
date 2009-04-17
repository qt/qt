load(qttest_p4) 
TEMPLATE = app
DEPENDPATH += .
INCLUDEPATH += .

# Input
SOURCES += tst_gui.cpp 

mac {
    SOURCES += guitest.cpp
    HEADERS += guitest.h
}



