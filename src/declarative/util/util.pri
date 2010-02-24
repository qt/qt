INCLUDEPATH += $$PWD

SOURCES += \
    $$PWD/qdeclarativeutilmodule.cpp\
    $$PWD/qdeclarativeview.cpp \
    $$PWD/qfxperf.cpp \
    $$PWD/qperformancelog.cpp \
    $$PWD/qdeclarativeconnection.cpp \
    $$PWD/qdeclarativepackage.cpp \
    $$PWD/qdeclarativeanimation.cpp \
    $$PWD/qdeclarativesystempalette.cpp \
    $$PWD/qdeclarativespringfollow.cpp \
    $$PWD/qdeclarativeeasefollow.cpp \
    $$PWD/qdeclarativestate.cpp\
    $$PWD/qdeclarativetransitionmanager.cpp \
    $$PWD/qdeclarativestateoperations.cpp \
    $$PWD/qdeclarativepropertychanges.cpp \
    $$PWD/qdeclarativestategroup.cpp \
    $$PWD/qdeclarativetransition.cpp \
    $$PWD/qdeclarativelistmodel.cpp\
    $$PWD/qdeclarativelistaccessor.cpp \
    $$PWD/qdeclarativeopenmetaobject.cpp \
    $$PWD/qdeclarativetimeline.cpp \
    $$PWD/qdeclarativetimer.cpp \
    $$PWD/qdeclarativebind.cpp \
    $$PWD/qdeclarativepropertymap.cpp \
    $$PWD/qdeclarativepixmapcache.cpp \
    $$PWD/qnumberformat.cpp \
    $$PWD/qdeclarativenumberformatter.cpp \
    $$PWD/qdeclarativedatetimeformatter.cpp \
    $$PWD/qdeclarativebehavior.cpp \
    $$PWD/qdeclarativefontloader.cpp \
    $$PWD/qdeclarativestyledtext.cpp \
    $$PWD/qdeclarativegraphicswidget.cpp

HEADERS += \
    $$PWD/qdeclarativeutilmodule_p.h\
    $$PWD/qdeclarativeview.h \
    $$PWD/qfxperf_p_p.h \
    $$PWD/qperformancelog_p_p.h \
    $$PWD/qdeclarativeconnection_p.h \
    $$PWD/qdeclarativepackage_p.h \
    $$PWD/qdeclarativeanimation_p.h \
    $$PWD/qdeclarativeanimation_p_p.h \
    $$PWD/qdeclarativesystempalette_p.h \
    $$PWD/qdeclarativespringfollow_p.h \
    $$PWD/qdeclarativeeasefollow_p.h \
    $$PWD/qdeclarativestate_p.h\
    $$PWD/qdeclarativestateoperations_p.h \
    $$PWD/qdeclarativepropertychanges_p.h \
    $$PWD/qdeclarativestate_p_p.h\
    $$PWD/qdeclarativetransitionmanager_p_p.h \
    $$PWD/qdeclarativestategroup_p.h \
    $$PWD/qdeclarativetransition_p.h \
    $$PWD/qdeclarativelistmodel_p.h\
    $$PWD/qdeclarativelistaccessor_p.h \
    $$PWD/qdeclarativeopenmetaobject_p.h \
    $$PWD/qdeclarativenullablevalue_p_p.h \
    $$PWD/qdeclarativetimeline_p_p.h \
    $$PWD/qdeclarativetimer_p.h \
    $$PWD/qdeclarativebind_p.h \
    $$PWD/qdeclarativepropertymap.h \
    $$PWD/qdeclarativepixmapcache_p.h \
    $$PWD/qnumberformat_p.h \
    $$PWD/qdeclarativenumberformatter_p.h \
    $$PWD/qdeclarativedatetimeformatter_p.h \
    $$PWD/qdeclarativebehavior_p.h \
    $$PWD/qdeclarativefontloader_p.h \
    $$PWD/qdeclarativestyledtext_p.h \
    $$PWD/qdeclarativegraphicswidget.h

contains(QT_CONFIG, xmlpatterns) {
    QT+=xmlpatterns
    SOURCES += $$PWD/qdeclarativexmllistmodel.cpp
    HEADERS += $$PWD/qdeclarativexmllistmodel_p.h
}
