HEADERS += stickman.h \
           animation.h \
           node.h \
           lifecycle.h \
           graphicsview.h
SOURCES += main.cpp \
           stickman.cpp \
           animation.cpp \
           node.cpp \
           lifecycle.cpp \
           graphicsview.cpp

INCLUDEPATH += $$PWD

include(editor/editor.pri)

# install
target.path = $$[QT_INSTALL_EXAMPLES]/animation/stickman
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS stickman.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/animation/stickman
INSTALLS += target sources
