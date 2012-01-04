DEFINES += QDOC2_COMPAT
DEFINES += QT_NO_CAST_TO_ASCII
#DEFINES += QT_NO_CAST_FROM_ASCII
#DEFINES += QT_USE_FAST_OPERATOR_PLUS
#DEFINES += QT_USE_FAST_CONCATENATION

qdoc_bootstrapped {
    include(../../src/tools/bootstrap/bootstrap.pri)
    SOURCES += ../../src/corelib/plugin/quuid.cpp
    DEFINES -= QT_NO_CAST_FROM_ASCII
    DEFINES += QT_NO_TRANSLATION
} else {
    QT = core xml
    CONFIG += console
    CONFIG -= debug_and_release_target
}

# Increase the stack size on MSVC to 4M to avoid a stack overflow 
win32-msvc*:{ 
    QMAKE_LFLAGS += /STACK:4194304
} 
 
!isEmpty(QT_BUILD_TREE):DESTDIR = $$QT_BUILD_TREE/bin
#CONFIG += debug
build_all:!build_pass {
    CONFIG -= build_all
    CONFIG += debug-and-release
#    CONFIG += debug
}

CONFIG -= app_bundle
HEADERS += atom.h \
           codechunk.h \
           codemarker.h \
	   codeparser.h \
           config.h \
	   cppcodemarker.h \
	   cppcodeparser.h \
	   ditaxmlgenerator.h \
           doc.h \
	   editdistance.h \
	   generator.h \
           helpprojectwriter.h \
	   htmlgenerator.h \
           jscodemarker.h \
           location.h \
           node.h \
           openedlist.h \
	   pagegenerator.h \
	   plaincodemarker.h \
           puredocparser.h \
           qmlcodemarker.h \
           qmlcodeparser.h \
           qmlmarkupvisitor.h \
           qmlvisitor.h \
           quoter.h \
	   separator.h \
	   text.h \
	   tokenizer.h \
	   tr.h \
	   tree.h
SOURCES += atom.cpp \
           codechunk.cpp \
           codemarker.cpp \
	   codeparser.cpp \
           config.cpp \
	   cppcodemarker.cpp \
	   cppcodeparser.cpp \
	   ditaxmlgenerator.cpp \
           doc.cpp \
	   editdistance.cpp \
	   generator.cpp \
           helpprojectwriter.cpp \
	   htmlgenerator.cpp \
           jscodemarker.cpp \
           location.cpp \
           main.cpp \
           node.cpp \
           openedlist.cpp \
	   pagegenerator.cpp \
	   plaincodemarker.cpp \
           puredocparser.cpp \
           qmlcodemarker.cpp \
           qmlcodeparser.cpp \
           qmlmarkupvisitor.cpp \
           qmlvisitor.cpp \
           quoter.cpp \
	   separator.cpp \
	   text.cpp \
	   tokenizer.cpp \
	   tree.cpp \
	   yyindent.cpp

INCLUDEPATH += $$QT_BUILD_TREE/include/QtDeclarative

include(declarativeparser/parser.pri)

### Documentation for qdoc3 ###

qtPrepareTool(QDOC, qdoc3)
qtPrepareTool(QHELPGENERATOR, qhelpgenerator)

equals(QMAKE_DIR_SEP, /) {
    QDOC = QT_BUILD_TREE=$$QT_BUILD_TREE QT_SOURCE_TREE=$$QT_SOURCE_TREE $$QDOC
} else {
    QDOC = set QT_BUILD_TREE=$$QT_BUILD_TREE&& set QT_SOURCE_TREE=$$QT_SOURCE_TREE&& $$QDOC
    QDOC = $$replace(QDOC, "/", "\\")
}

html-docs.commands = cd \"$$QT_BUILD_TREE/doc\" && $$QDOC $$QT_SOURCE_TREE/tools/qdoc3/doc/config/qdoc.qdocconf
html-docs.files = $$QT_BUILD_TREE/doc/html

qch-docs.commands = cd \"$$QT_BUILD_TREE/doc\" && $$QHELPGENERATOR $$QT_BUILD_TREE/tools/qdoc3/doc/html/qdoc.qhp -o $$QT_BUILD_TREE/tools/qdoc3/doc/qch/qdoc.qch
qch-docs.files = $$QT_BUILD_TREE/tools/qdoc3/doc/qch
qch-docs.path = $$[QT_INSTALL_DOCS]
qch-docs.CONFIG += no_check_exist directory

QMAKE_EXTRA_TARGETS += html-docs qch-docs

target.path = $$[QT_INSTALL_BINS]
INSTALLS += target
