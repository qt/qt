SOURCES += \
           extra/qnumberformat.cpp \
           extra/qmlnumberformatter.cpp \
           extra/qmldatetimeformatter.cpp \
           extra/qfxintegermodel.cpp \
           extra/qmlfolderlistmodel.cpp \
           extra/qmltimer.cpp \
           extra/qfxanimatedimageitem.cpp \
           extra/qfxflowview.cpp \
           extra/qfxparticles.cpp \
           extra/qmlbehaviour.cpp \
           extra/qbindablemap.cpp

HEADERS += \
           extra/qnumberformat.h \
           extra/qmlnumberformatter.h \
           extra/qmldatetimeformatter.h \
           extra/qfxintegermodel.h \
           extra/qmlfolderlistmodel.h \
           extra/qmltimer.h \
           extra/qfxanimatedimageitem.h \
           extra/qfxanimatedimageitem_p.h \
           extra/qfxflowview.h \
           extra/qfxparticles.h \
           extra/qmlbehaviour.h \
           extra/qbindablemap.h

contains(QT_CONFIG, xmlpatterns) {
    QT+=xmlpatterns
    SOURCES += extra/qmlxmllistmodel.cpp
    HEADERS += extra/qmlxmllistmodel.h
}

# SQL is permanently enabled :-/
#contains(QT_CONFIG, sql) {
    QT+= sql
    SOURCES += extra/qmlsqlquery.cpp \
            extra/qmlsqlconnection.cpp
    HEADERS += extra/qmlsqlquery.h \
            extra/qmlsqlconnection.h
#}
