SOURCES += \
           extra/qnumberformat.cpp \
           extra/qmlnumberformatter.cpp \
           extra/qmldatetimeformatter.cpp \
           extra/qmlgraphicsintegermodel.cpp \
           extra/qmlgraphicsanimatedimageitem.cpp \
           extra/qmlgraphicsparticles.cpp \
           extra/qmlbehavior.cpp \
           extra/qmlfontloader.cpp

HEADERS += \
           extra/qnumberformat.h \
           extra/qmlnumberformatter.h \
           extra/qmldatetimeformatter.h \
           extra/qmlgraphicsintegermodel.h \
           extra/qmlgraphicsanimatedimageitem.h \
           extra/qmlgraphicsanimatedimageitem_p.h \
           extra/qmlgraphicsparticles.h \
           extra/qmlbehavior.h \
           extra/qmlfontloader.h

contains(QT_CONFIG, xmlpatterns) {
    QT+=xmlpatterns
    SOURCES += extra/qmlxmllistmodel.cpp
    HEADERS += extra/qmlxmllistmodel.h
}

