SOURCES += qml/qmlparser.cpp \
    qml/qmlinstruction.cpp \
    qml/qmlvmemetaobject.cpp \
    qml/qmlengine.cpp \
    qml/qmlexpression.cpp \
    qml/qmlbinding.cpp \
    qml/qmlmetaproperty.cpp \
    qml/qmlcomponentjs.cpp \
    qml/qmlcomponent.cpp \
    qml/qmlcontext.cpp \
    qml/qmlcustomparser.cpp \
    qml/qmlpropertyvaluesource.cpp \
    qml/qmlproxymetaobject.cpp \
    qml/qmlvme.cpp \
    qml/qmlcompiler.cpp \
    qml/qmlcompileddata.cpp \
    qml/qmlboundsignal.cpp \
    qml/qmldom.cpp \
    qml/qmlrefcount.cpp \
    qml/qmlprivate.cpp \
    qml/qmlmetatype.cpp \
    qml/qmlstringconverters.cpp \
    qml/qmlclassfactory.cpp \
    qml/qmlparserstatus.cpp \
    qml/qmlcompositetypemanager.cpp \
    qml/qmlinfo.cpp \
    qml/qmlerror.cpp \
    qml/qmlscriptparser.cpp \
    qml/qmlenginedebug.cpp \
    qml/qmlrewrite.cpp \
    qml/qmlbasicscript.cpp \
    qml/qmlvaluetype.cpp \
    qml/qmlbindingoptimizations.cpp

HEADERS += qml/qmlparser_p.h \
    qml/qmlinstruction_p.h \
    qml/qmlvmemetaobject_p.h \
    qml/qml.h \
    qml/qmlbinding.h \
    qml/qmlbinding_p.h \
    qml/qmlmetaproperty.h \
    qml/qmlcomponentjs_p.h \
    qml/qmlcomponentjs_p_p.h \
    qml/qmlcomponent.h \
    qml/qmlcomponent_p.h \
    qml/qmlcustomparser_p.h \
    qml/qmlcustomparser_p_p.h \
    qml/qmlpropertyvaluesource.h \
    qml/qmlboundsignal_p.h \
    qml/qmlparserstatus.h \
    qml/qmlproxymetaobject_p.h \
    qml/qmlvme_p.h \
    qml/qmlcompiler_p.h \
    qml/qmlengine_p.h \
    qml/qmlexpression_p.h \
    qml/qmlprivate.h \
    qml/qmldom.h \
    qml/qmldom_p.h \
    qml/qmlrefcount_p.h \
    qml/qmlmetatype.h \
    qml/qmlengine.h \
    qml/qmlcontext.h \
    qml/qmlexpression.h \
    qml/qmlstringconverters_p.h \
    qml/qmlclassfactory_p.h \
    qml/qmlinfo.h \
    qml/qmlmetaproperty_p.h \
    qml/qmlcontext_p.h \
    qml/qmlcompositetypedata_p.h \
    qml/qmlcompositetypemanager_p.h \
    qml/qmllist.h \
    qml/qmldeclarativedata_p.h \
    qml/qmlerror.h \
    qml/qmlscriptparser_p.h \
    qml/qmlbasicscript_p.h \
    qml/qmlenginedebug_p.h \
    qml/qmlrewrite_p.h \
    qml/qpodvector_p.h \
    qml/qmlvaluetype_p.h \
    qml/qmlbindingoptimizations_p.h

# for qtscript debugger
contains(QT_CONFIG, scripttools):QT += scripttools

include(script/script.pri)
include(parser/parser.pri)
include(rewriter/rewriter.pri)

