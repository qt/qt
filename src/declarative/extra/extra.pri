SOURCES += \
           extra/qnumberformat.cpp \
           extra/qmlnumberformatter.cpp \
           extra/qmldatetimeformatter.cpp \
           extra/qfxintegermodel.cpp \
           extra/qfxanimatedimageitem.cpp \
           extra/qfxparticles.cpp \
           extra/qmlbehavior.cpp \
           extra/qmlfontloader.cpp

HEADERS += \
           extra/qnumberformat.h \
           extra/qmlnumberformatter.h \
           extra/qmldatetimeformatter.h \
           extra/qfxintegermodel.h \
           extra/qfxanimatedimageitem.h \
           extra/qfxanimatedimageitem_p.h \
           extra/qfxparticles.h \
           extra/qmlbehavior.h \
           extra/qmlfontloader.h

contains(QT_CONFIG, xmlpatterns) {
    QT+=xmlpatterns
    SOURCES += extra/qmlxmllistmodel.cpp
    HEADERS += extra/qmlxmllistmodel.h
}

