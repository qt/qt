INCLUDEPATH += $$PWD

SOURCES += \
    $$PWD/qmlview.cpp \
    $$PWD/qfxperf.cpp \
    $$PWD/qperformancelog.cpp \
    $$PWD/qmlconnection.cpp \
    $$PWD/qmlpackage.cpp \
    $$PWD/qmlanimation.cpp \
    $$PWD/qmlsystempalette.cpp \
    $$PWD/qmlspringfollow.cpp \
    $$PWD/qmleasefollow.cpp \
    $$PWD/qmlstate.cpp\
    $$PWD/qmltransitionmanager.cpp \
    $$PWD/qmlstateoperations.cpp \
    $$PWD/qmlpropertychanges.cpp \
    $$PWD/qmlstategroup.cpp \
    $$PWD/qmltransition.cpp \
    $$PWD/qmllistmodel.cpp\
    $$PWD/qmllistaccessor.cpp \
    $$PWD/qmlopenmetaobject.cpp \
    $$PWD/qmltimeline.cpp \
    $$PWD/qmltimer.cpp \
    $$PWD/qmlbind.cpp \
    $$PWD/qmlpropertymap.cpp \
    $$PWD/qmlpixmapcache.cpp \
    $$PWD/qnumberformat.cpp \
    $$PWD/qmlnumberformatter.cpp \
    $$PWD/qmldatetimeformatter.cpp \
    $$PWD/qmlbehavior.cpp \
    $$PWD/qmlfontloader.cpp \
    $$PWD/qmlstyledtext.cpp

HEADERS += \
    $$PWD/qmlview.h \
    $$PWD/qfxperf_p_p.h \
    $$PWD/qperformancelog_p_p.h \
    $$PWD/qmlconnection_p.h \
    $$PWD/qmlpackage_p.h \
    $$PWD/qmlanimation_p.h \
    $$PWD/qmlanimation_p_p.h \
    $$PWD/qmlsystempalette_p.h \
    $$PWD/qmlspringfollow_p.h \
    $$PWD/qmleasefollow_p.h \
    $$PWD/qmlstate_p.h\
    $$PWD/qmlstateoperations_p.h \
    $$PWD/qmlpropertychanges_p.h \
    $$PWD/qmlstate_p_p.h\
    $$PWD/qmltransitionmanager_p_p.h \
    $$PWD/qmlstategroup_p.h \
    $$PWD/qmltransition_p.h \
    $$PWD/qmllistmodel_p.h\
    $$PWD/qmllistaccessor_p.h \
    $$PWD/qmlopenmetaobject_p.h \
    $$PWD/qmlnullablevalue_p_p.h \
    $$PWD/qmltimeline_p_p.h \
    $$PWD/qmltimer_p.h \
    $$PWD/qmlbind_p.h \
    $$PWD/qmlpropertymap.h \
    $$PWD/qmlpixmapcache_p.h \
    $$PWD/qnumberformat_p.h \
    $$PWD/qmlnumberformatter_p.h \
    $$PWD/qmldatetimeformatter_p.h \
    $$PWD/qmlbehavior_p.h \
    $$PWD/qmlfontloader_p.h \
    $$PWD/qmlstyledtext_p.h

contains(QT_CONFIG, xmlpatterns) {
    QT+=xmlpatterns
    SOURCES += $$PWD/qmlxmllistmodel.cpp
    HEADERS += $$PWD/qmlxmllistmodel_p.h
}
