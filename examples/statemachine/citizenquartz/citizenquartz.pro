TEMPLATE = app
TARGET = 
DEPENDPATH += .
INCLUDEPATH += .

# Input
SOURCES += main.cpp \ 
          clock.cpp \ 
		  clockbutton.cpp \
		  clockdisplay.cpp \
		  propertyaddstate.cpp \
		  
HEADERS += clock.h \
           clockbutton.h \
		   clockdisplay.h \
		   propertyaddstate.h \
           timeperiod.h \           

RESOURCES += citizenquartz.qrc
CONFIG += console
