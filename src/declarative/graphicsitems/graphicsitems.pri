HEADERS += \
           graphicsitems/qmlgraphicsanchors_p.h \
           graphicsitems/qmlgraphicsanchors_p_p.h \
           graphicsitems/qmlgraphicsevents_p_p.h \
           graphicsitems/qmlgraphicsflickable_p.h \
           graphicsitems/qmlgraphicsflickable_p_p.h \
           graphicsitems/qmlgraphicsflipable_p.h \
           graphicsitems/qmlgraphicsgridview_p.h \
           graphicsitems/qmlgraphicsimage_p.h \
           graphicsitems/qmlgraphicsimagebase_p.h \
           graphicsitems/qmlgraphicsborderimage_p.h \
           graphicsitems/qmlgraphicspainteditem_p.h \
           graphicsitems/qmlgraphicspainteditem_p_p.h \
           graphicsitems/qmlgraphicsimage_p_p.h \
           graphicsitems/qmlgraphicsborderimage_p_p.h \
           graphicsitems/qmlgraphicsimagebase_p_p.h \
           graphicsitems/qmlgraphicsitem.h \
           graphicsitems/qmlgraphicsitem_p.h \
           graphicsitems/qmlgraphicsfocuspanel_p.h \
           graphicsitems/qmlgraphicsfocusscope_p.h \
           graphicsitems/qmlgraphicspositioners_p.h \
           graphicsitems/qmlgraphicspositioners_p_p.h \
           graphicsitems/qmlgraphicsloader_p.h \
           graphicsitems/qmlgraphicsloader_p_p.h \
           graphicsitems/qmlgraphicsmouseregion_p.h \
           graphicsitems/qmlgraphicsmouseregion_p_p.h \
           graphicsitems/qmlgraphicspath_p.h \
           graphicsitems/qmlgraphicspath_p_p.h \
           graphicsitems/qmlgraphicspathview_p.h \
           graphicsitems/qmlgraphicspathview_p_p.h \
           graphicsitems/qmlgraphicsrectangle_p.h \
           graphicsitems/qmlgraphicsrectangle_p_p.h \
           graphicsitems/qmlgraphicsrepeater_p.h \
           graphicsitems/qmlgraphicsrepeater_p_p.h \
           graphicsitems/qmlgraphicsscalegrid_p_p.h \
           graphicsitems/qmlgraphicstextinput_p.h \
           graphicsitems/qmlgraphicstextinput_p_p.h \
           graphicsitems/qmlgraphicstextedit_p.h \
           graphicsitems/qmlgraphicstextedit_p_p.h \
           graphicsitems/qmlgraphicstext_p.h \
           graphicsitems/qmlgraphicstext_p_p.h \
           graphicsitems/qmlgraphicspixmapcache_p.h \
           graphicsitems/qmlgraphicsvisualitemmodel_p.h \
           graphicsitems/qmlgraphicslistview_p.h \
           graphicsitems/qmlgraphicsgraphicsobjectcontainer_p.h \
           graphicsitems/qmlgraphicslayoutitem_p.h \
           graphicsitems/qmlgraphicseffects.cpp

SOURCES += \
           graphicsitems/qmlgraphicsanchors.cpp \
           graphicsitems/qmlgraphicsevents.cpp \
           graphicsitems/qmlgraphicsflickable.cpp \
           graphicsitems/qmlgraphicsflipable.cpp \
           graphicsitems/qmlgraphicsgridview.cpp \
           graphicsitems/qmlgraphicsimage.cpp \
           graphicsitems/qmlgraphicsborderimage.cpp \
           graphicsitems/qmlgraphicsimagebase.cpp \
           graphicsitems/qmlgraphicspainteditem.cpp \
           graphicsitems/qmlgraphicsitem.cpp \
           graphicsitems/qmlgraphicsfocuspanel.cpp \
           graphicsitems/qmlgraphicsfocusscope.cpp \
           graphicsitems/qmlgraphicspositioners.cpp \
           graphicsitems/qmlgraphicsloader.cpp \
           graphicsitems/qmlgraphicsmouseregion.cpp \
           graphicsitems/qmlgraphicspath.cpp \
           graphicsitems/qmlgraphicspathview.cpp \
           graphicsitems/qmlgraphicsrectangle.cpp \
           graphicsitems/qmlgraphicsrepeater.cpp \
           graphicsitems/qmlgraphicsscalegrid.cpp \
           graphicsitems/qmlgraphicstextinput.cpp \
           graphicsitems/qmlgraphicstext.cpp \
           graphicsitems/qmlgraphicstextedit.cpp \
           graphicsitems/qmlgraphicspixmapcache.cpp \
           graphicsitems/qmlgraphicsvisualitemmodel.cpp \
           graphicsitems/qmlgraphicslistview.cpp \
           graphicsitems/qmlgraphicsgraphicsobjectcontainer.cpp \
           graphicsitems/qmlgraphicslayoutitem.cpp \

contains(QT_CONFIG, webkit) {
    QT+=webkit
    SOURCES += graphicsitems/qmlgraphicswebview.cpp
    HEADERS += graphicsitems/qmlgraphicswebview_p.h
}

