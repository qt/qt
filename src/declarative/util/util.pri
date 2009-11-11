SOURCES += \
           util/qmlview.cpp \
           util/qfxperf.cpp \
           util/qperformancelog.cpp \
           util/qmlconnection.cpp \
           util/qmlpackage.cpp \
           util/qmlanimation.cpp \
           util/qmlsystempalette.cpp \
           util/qmlspringfollow.cpp \
           util/qmleasefollow.cpp \
           util/qmlstate.cpp\
           util/qmltransitionmanager.cpp \
           util/qmlstateoperations.cpp \
           util/qmlpropertychanges.cpp \
           util/qmlstategroup.cpp \
           util/qmltransition.cpp \
           util/qmllistmodel.cpp\
           util/qmllistaccessor.cpp \
           util/qmlopenmetaobject.cpp \
           util/qmltimeline.cpp \
           util/qmltimer.cpp \
           util/qmlbind.cpp \
           util/qmlpropertymap.cpp \
           util/qmlpixmapcache.cpp \
           util/qnumberformat.cpp \
           util/qmlnumberformatter.cpp \
           util/qmldatetimeformatter.cpp \
           util/qmlbehavior.cpp \
           util/qmlfontloader.cpp

HEADERS += \
           util/qmlview.h \
           util/qfxperf_p_p.h \
           util/qperformancelog_p_p.h \
           util/qmlconnection_p.h \
           util/qmlpackage_p.h \
           util/qmlanimation_p.h \
           util/qmlanimation_p_p.h \
           util/qmlsystempalette_p.h \
           util/qmlspringfollow_p.h \
           util/qmleasefollow_p.h \
           util/qmlstate_p.h\
           util/qmlstateoperations_p.h \
           util/qmlpropertychanges_p.h \
           util/qmlstate_p_p.h\
           util/qmltransitionmanager_p_p.h \
           util/qmlstategroup_p.h \
           util/qmltransition_p.h \
           util/qmllistmodel_p.h\
           util/qmllistaccessor_p.h \
           util/qmlopenmetaobject_p.h \
           util/qmlnullablevalue_p_p.h \
           util/qmltimeline_p_p.h \
           util/qmltimer_p.h \
           util/qmlbind_p.h \
           util/qmlpropertymap_p.h \
           util/qmlpixmapcache_p.h \
           util/qnumberformat_p.h \
           util/qmlnumberformatter_p.h \
           util/qmldatetimeformatter_p.h \
           util/qmlbehavior_p.h \
           util/qmlfontloader_p.h

contains(QT_CONFIG, xmlpatterns) {
    QT+=xmlpatterns
    SOURCES += util/qmlxmllistmodel.cpp
    HEADERS += util/qmlxmllistmodel_p.h
}
