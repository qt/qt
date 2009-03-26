FORMS     = forms/window.ui
HEADERS   = displaywidget.h \
            window.h
RESOURCES = svggenerator.qrc
SOURCES   = displaywidget.cpp \
            main.cpp \
            window.cpp

QT += svg

# install
target.path = $$[QT_INSTALL_EXAMPLES]/painting/svggenerator
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS svggenerator.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/painting/svggenerator
INSTALLS += target sources
