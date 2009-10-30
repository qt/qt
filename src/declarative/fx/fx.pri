HEADERS += \
           fx/qmlgraphicsanchors.h \
           fx/qmlgraphicsanchors_p.h \
           fx/qmlgraphicsevents_p.h \
           fx/qmlgraphicsflickable.h \
           fx/qmlgraphicsflickable_p.h \
           fx/qmlgraphicsflipable.h \
           fx/qmlgraphicsgridview.h \
           fx/qmlgraphicsimage.h \
           fx/qmlgraphicsimagebase.h \
           fx/qmlgraphicsborderimage.h \
           fx/qmlgraphicspainteditem.h \
           fx/qmlgraphicspainteditem_p.h \
           fx/qmlgraphicsimage_p.h \
           fx/qmlgraphicsborderimage_p.h \
           fx/qmlgraphicsimagebase_p.h \
           fx/qmlgraphicsitem.h \
           fx/qmlgraphicsitem_p.h \
           fx/qmlgraphicsfocuspanel.h \
           fx/qmlgraphicsfocusscope.h \
           fx/qmlgraphicspositioners.h \
           fx/qmlgraphicspositioners_p.h \
           fx/qmlgraphicsloader.h \
           fx/qmlgraphicsloader_p.h \
           fx/qmlgraphicsmouseregion.h \
           fx/qmlgraphicsmouseregion_p.h \
           fx/qmlgraphicspath.h \
           fx/qmlgraphicspath_p.h \
           fx/qmlgraphicspathview.h \
           fx/qmlgraphicspathview_p.h \
           fx/qmlgraphicsrect.h \
           fx/qmlgraphicsrect_p.h \
           fx/qmlgraphicsrepeater.h \
           fx/qmlgraphicsrepeater_p.h \
           fx/qmlgraphicsscalegrid_p.h \
           fx/qmlgraphicstextinput.h \
           fx/qmlgraphicstextinput_p.h \
           fx/qmlgraphicstextedit.h \
           fx/qmlgraphicstextedit_p.h \
           fx/qmlgraphicstext.h \
           fx/qmlgraphicstext_p.h \
           fx/qmlgraphicspixmapcache.h \
           fx/qmlgraphicsvisualitemmodel.h \
           fx/qmlgraphicslistview.h \
           fx/qmlgraphicsgraphicsobjectcontainer.h \
           fx/qmlgraphicslayoutitem.h \
           fx/qmlgraphicseffects.cpp

SOURCES += \
           fx/qmlgraphicsanchors.cpp \
           fx/qmlgraphicsevents.cpp \
           fx/qmlgraphicsflickable.cpp \
           fx/qmlgraphicsflipable.cpp \
           fx/qmlgraphicsgridview.cpp \
           fx/qmlgraphicsimage.cpp \
           fx/qmlgraphicsborderimage.cpp \
           fx/qmlgraphicsimagebase.cpp \
           fx/qmlgraphicspainteditem.cpp \
           fx/qmlgraphicsitem.cpp \
           fx/qmlgraphicsfocuspanel.cpp \
           fx/qmlgraphicsfocusscope.cpp \
           fx/qmlgraphicspositioners.cpp \
           fx/qmlgraphicsloader.cpp \
           fx/qmlgraphicsmouseregion.cpp \
           fx/qmlgraphicspath.cpp \
           fx/qmlgraphicspathview.cpp \
           fx/qmlgraphicsrect.cpp \
           fx/qmlgraphicsrepeater.cpp \
           fx/qmlgraphicsscalegrid.cpp \
           fx/qmlgraphicstextinput.cpp \
           fx/qmlgraphicstext.cpp \
           fx/qmlgraphicstextedit.cpp \
           fx/qmlgraphicspixmapcache.cpp \
           fx/qmlgraphicsvisualitemmodel.cpp \
           fx/qmlgraphicslistview.cpp \
           fx/qmlgraphicsgraphicsobjectcontainer.cpp \
           fx/qmlgraphicslayoutitem.cpp \

contains(QT_CONFIG, webkit) {
    QT+=webkit
    SOURCES += fx/qmlgraphicswebview.cpp
    HEADERS += fx/qmlgraphicswebview.h
}

