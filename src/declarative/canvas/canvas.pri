SOURCES += \
           canvas/qsimplecanvas.cpp \
           canvas/qsimplecanvasitem.cpp \
           canvas/qsimplecanvasfilter.cpp \
           canvas/qsimplecanvasdebugplugin.cpp

HEADERS += \
           canvas/qsimplecanvas.h \
           canvas/qsimplecanvasitem.h \
           canvas/qsimplecanvasfilter.h \
           canvas/qsimplecanvas_p.h \
           canvas/qsimplecanvasitem_p.h \
           canvas/qsimplecanvasfilter_p.h \
           canvas/qsimplecanvasdebugplugin_p.h

contains(QT_CONFIG, opengles2): SOURCES += canvas/qsimplecanvas_opengl.cpp
else:contains(QT_CONFIG, opengles1): SOURCES += canvas/qsimplecanvas_opengl1.cpp
else:SOURCES += canvas/qsimplecanvas_software.cpp
SOURCES += canvas/qsimplecanvas_graphicsview.cpp
