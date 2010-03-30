TEMPLATE = app
TARGET = BearerEx

QT += core \
      gui \
      network

FORMS += sessiondialog.ui \
         bearerex.ui \
         detailedinfodialog.ui

# Example headers and sources
HEADERS += bearerex.h \
           xqlistwidget.h
    
SOURCES += bearerex.cpp \
           main.cpp \
           xqlistwidget.cpp

symbian:TARGET.CAPABILITY = NetworkServices NetworkControl ReadUserData