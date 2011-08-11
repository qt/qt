HEADERS     = mainwindow.h
SOURCES     = main.cpp \
              mainwindow.cpp

# App cannot be with name "calendar" in Symbian due to same named system component.
symbian: TARGET = calendarapp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/richtext/calendar
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS calendar.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/richtext/calendar
INSTALLS += target sources

symbian: include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
maemo5: include($$QT_SOURCE_TREE/examples/maemo5pkgrules.pri)

