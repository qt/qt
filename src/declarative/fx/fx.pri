HEADERS += \
           fx/qfxanchors.h \
           fx/qfxanchors_p.h \
           fx/qfxcomponentinstance.h \
           fx/qfxcomponentinstance_p.h \
           fx/qfxevents_p.h \
           fx/qfxflickable.h \
           fx/qfxflickable_p.h \
           fx/qfxflipable.h \
           fx/qfxgridview.h \
           fx/qfximage.h \
           fx/qfxpainteditem.h \
           fx/qfxpainteditem_p.h \
           fx/qfximage_p.h \
           fx/qfxitem.h \
           fx/qfxitem_p.h \
           fx/qfxfocusrealm.h \
           fx/qfxkeyactions.h \
           fx/qfxkeyproxy.h \
           fx/qfxlayouts.h \
           fx/qfxlayouts_p.h \
           fx/qfxmouseregion.h \
           fx/qfxmouseregion_p.h \
           fx/qfxpath.h \
           fx/qfxpath_p.h \
           fx/qfxpathview.h \
           fx/qfxpathview_p.h \
           fx/qfxrect.h \
           fx/qfxrect_p.h \
           fx/qfxrepeater.h \
           fx/qfxrepeater_p.h \
           fx/qfxscalegrid.h \
           fx/qfxlineedit.h \
           fx/qfxlineedit_p.h \
           fx/qfxtextedit.h \
           fx/qfxtextedit_p.h \
           fx/qfxtext.h \
           fx/qfxtext_p.h \
           fx/qfxtransform.h \
           fx/qfxpixmap.cpp \
           fx/qfxvisualitemmodel.h \
           fx/qfxlistview.h \
           fx/qfxgraphicsobjectcontainer.h \

SOURCES += \
           fx/qfxanchors.cpp \
           fx/qfxcomponentinstance.cpp \
           fx/qfxevents.cpp \
           fx/qfxflickable.cpp \
           fx/qfxflipable.cpp \
           fx/qfxgridview.cpp \
           fx/qfximage.cpp \
           fx/qfxpainteditem.cpp \
           fx/qfxitem.cpp \
           fx/qfxfocusrealm.cpp \
           fx/qfxkeyactions.cpp \
           fx/qfxkeyproxy.cpp \
           fx/qfxlayouts.cpp \
           fx/qfxmouseregion.cpp \
           fx/qfxpath.cpp \
           fx/qfxpathview.cpp \
           fx/qfxrect.cpp \
           fx/qfxrepeater.cpp \
           fx/qfxscalegrid.cpp \
           fx/qfxlineedit.cpp \
           fx/qfxtext.cpp \
           fx/qfxtextedit.cpp \
           fx/qfxtransform.cpp \
           fx/qfxpixmap.cpp \
           fx/qfxvisualitemmodel.cpp \
           fx/qfxlistview.cpp \
           fx/qfxgraphicsobjectcontainer.cpp \

contains(QT_CONFIG, webkit) {
    QT+=webkit
    SOURCES += fx/qfxwebview.cpp
    HEADERS += fx/qfxwebview.h
}

