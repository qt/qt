TEMPLATE = subdirs
SUBDIRS = qdbus qdbusxml2cpp qdbuscpp2xml
!contains(QT_CONFIG, no-gui): SUBDIRS += qdbusviewer
