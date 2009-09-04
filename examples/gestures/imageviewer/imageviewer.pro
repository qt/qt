HEADERS += imagewidget.h \
    tapandholdgesture.h
SOURCES += imagewidget.cpp \
    tapandholdgesture.cpp \
    main.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/gestures/imageviewer
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS imageviewer.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/gestures/imageviewer
INSTALLS += target sources
