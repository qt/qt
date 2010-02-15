INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/qmlgraphicsitemsmodule_p.h \
    $$PWD/qmlgraphicsanchors_p.h \
    $$PWD/qmlgraphicsanchors_p_p.h \
    $$PWD/qmlgraphicsevents_p_p.h \
    $$PWD/qmlgraphicseffects_p.h \
    $$PWD/qmlgraphicsflickable_p.h \
    $$PWD/qmlgraphicsflickable_p_p.h \
    $$PWD/qmlgraphicsflipable_p.h \
    $$PWD/qmlgraphicsgridview_p.h \
    $$PWD/qmlgraphicsimage_p.h \
    $$PWD/qmlgraphicsimagebase_p.h \
    $$PWD/qmlgraphicsborderimage_p.h \
    $$PWD/qmlgraphicspainteditem_p.h \
    $$PWD/qmlgraphicspainteditem_p_p.h \
    $$PWD/qmlgraphicsimage_p_p.h \
    $$PWD/qmlgraphicsborderimage_p_p.h \
    $$PWD/qmlgraphicsimagebase_p_p.h \
    $$PWD/qmlgraphicsanimatedimage_p.h \
    $$PWD/qmlgraphicsanimatedimage_p_p.h \
    $$PWD/qmlgraphicsitem.h \
    $$PWD/qmlgraphicsitem_p.h \
    $$PWD/qmlgraphicsfocuspanel_p.h \
    $$PWD/qmlgraphicsfocusscope_p.h \
    $$PWD/qmlgraphicspositioners_p.h \
    $$PWD/qmlgraphicspositioners_p_p.h \
    $$PWD/qmlgraphicsloader_p.h \
    $$PWD/qmlgraphicsloader_p_p.h \
    $$PWD/qmlgraphicsmouseregion_p.h \
    $$PWD/qmlgraphicsmouseregion_p_p.h \
    $$PWD/qmlgraphicspath_p.h \
    $$PWD/qmlgraphicspath_p_p.h \
    $$PWD/qmlgraphicspathview_p.h \
    $$PWD/qmlgraphicspathview_p_p.h \
    $$PWD/qmlgraphicsrectangle_p.h \
    $$PWD/qmlgraphicsrectangle_p_p.h \
    $$PWD/qmlgraphicsrepeater_p.h \
    $$PWD/qmlgraphicsrepeater_p_p.h \
    $$PWD/qmlgraphicsscalegrid_p_p.h \
    $$PWD/qmlgraphicstextinput_p.h \
    $$PWD/qmlgraphicstextinput_p_p.h \
    $$PWD/qmlgraphicstextedit_p.h \
    $$PWD/qmlgraphicstextedit_p_p.h \
    $$PWD/qmlgraphicstext_p.h \
    $$PWD/qmlgraphicstext_p_p.h \
    $$PWD/qmlgraphicsvisualitemmodel_p.h \
    $$PWD/qmlgraphicslistview_p.h \
    $$PWD/qmlgraphicsgraphicsobjectcontainer_p.h \
    $$PWD/qmlgraphicsparticles_p.h \
    $$PWD/qmlgraphicslayoutitem_p.h \
    $$PWD/qmlgraphicsitemchangelistener_p.h \
    $$PWD/qmlgraphicseffects.cpp

SOURCES += \
    $$PWD/qmlgraphicsitemsmodule.cpp \
    $$PWD/qmlgraphicsanchors.cpp \
    $$PWD/qmlgraphicsevents.cpp \
    $$PWD/qmlgraphicsflickable.cpp \
    $$PWD/qmlgraphicsflipable.cpp \
    $$PWD/qmlgraphicsgridview.cpp \
    $$PWD/qmlgraphicsimage.cpp \
    $$PWD/qmlgraphicsborderimage.cpp \
    $$PWD/qmlgraphicsimagebase.cpp \
    $$PWD/qmlgraphicsanimatedimage.cpp \
    $$PWD/qmlgraphicspainteditem.cpp \
    $$PWD/qmlgraphicsitem.cpp \
    $$PWD/qmlgraphicsfocuspanel.cpp \
    $$PWD/qmlgraphicsfocusscope.cpp \
    $$PWD/qmlgraphicspositioners.cpp \
    $$PWD/qmlgraphicsloader.cpp \
    $$PWD/qmlgraphicsmouseregion.cpp \
    $$PWD/qmlgraphicspath.cpp \
    $$PWD/qmlgraphicspathview.cpp \
    $$PWD/qmlgraphicsrectangle.cpp \
    $$PWD/qmlgraphicsrepeater.cpp \
    $$PWD/qmlgraphicsscalegrid.cpp \
    $$PWD/qmlgraphicstextinput.cpp \
    $$PWD/qmlgraphicstext.cpp \
    $$PWD/qmlgraphicstextedit.cpp \
    $$PWD/qmlgraphicsvisualitemmodel.cpp \
    $$PWD/qmlgraphicslistview.cpp \
    $$PWD/qmlgraphicsgraphicsobjectcontainer.cpp \
    $$PWD/qmlgraphicsparticles.cpp \
    $$PWD/qmlgraphicslayoutitem.cpp \

contains(QT_CONFIG, webkit) {
    QT+=webkit
    SOURCES += $$PWD/qmlgraphicswebview.cpp
    HEADERS += $$PWD/qmlgraphicswebview_p.h
    HEADERS += $$PWD/qmlgraphicswebview_p_p.h
}
