HEADERS += \
           fx/qmlgraphicsanchors_p.h \
           fx/qmlgraphicsanchors_p_p.h \
           fx/qmlgraphicsevents_p_p.h \
           fx/qmlgraphicsflickable_p.h \
           fx/qmlgraphicsflickable_p_p.h \
           fx/qmlgraphicsflipable_p.h \
           fx/qmlgraphicsgridview_p.h \
           fx/qmlgraphicsimage_p.h \
           fx/qmlgraphicsimagebase_p.h \
           fx/qmlgraphicsborderimage_p.h \
           fx/qmlgraphicspainteditem_p.h \
           fx/qmlgraphicspainteditem_p_p.h \
           fx/qmlgraphicsimage_p_p.h \
           fx/qmlgraphicsborderimage_p_p.h \
           fx/qmlgraphicsimagebase_p_p.h \
           fx/qmlgraphicsitem.h \
           fx/qmlgraphicsitem_p.h \
           fx/qmlgraphicsfocuspanel_p.h \
           fx/qmlgraphicsfocusscope_p.h \
           fx/qmlgraphicspositioners_p.h \
           fx/qmlgraphicspositioners_p_p.h \
           fx/qmlgraphicsloader_p.h \
           fx/qmlgraphicsloader_p_p.h \
           fx/qmlgraphicsmouseregion_p.h \
           fx/qmlgraphicsmouseregion_p_p.h \
           fx/qmlgraphicspath_p.h \
           fx/qmlgraphicspath_p_p.h \
           fx/qmlgraphicspathview_p.h \
           fx/qmlgraphicspathview_p_p.h \
           fx/qmlgraphicsrect_p.h \
           fx/qmlgraphicsrect_p_p.h \
           fx/qmlgraphicsrepeater_p.h \
           fx/qmlgraphicsrepeater_p_p.h \
           fx/qmlgraphicsscalegrid_p_p.h \
           fx/qmlgraphicstextinput_p.h \
           fx/qmlgraphicstextinput_p_p.h \
           fx/qmlgraphicstextedit_p.h \
           fx/qmlgraphicstextedit_p_p.h \
           fx/qmlgraphicstext_p.h \
           fx/qmlgraphicstext_p_p.h \
           fx/qmlgraphicspixmapcache_p.h \
           fx/qmlgraphicsvisualitemmodel_p.h \
           fx/qmlgraphicslistview_p.h \
           fx/qmlgraphicsgraphicsobjectcontainer_p.h \
           fx/qmlgraphicslayoutitem_p.h \
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
    HEADERS += fx/qmlgraphicswebview_p.h
}

