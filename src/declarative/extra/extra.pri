SOURCES += \
           extra/qnumberformat.cpp \
           extra/qmlnumberformatter.cpp \
           extra/qfxintegermodel.cpp \
           extra/qmltimer.cpp

HEADERS += \
           extra/qnumberformat.h \
           extra/qmlnumberformatter.h \
           extra/qfxintegermodel.h \
           extra/qmltimer.h

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
