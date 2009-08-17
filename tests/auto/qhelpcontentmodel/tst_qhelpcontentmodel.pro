load(qttest_p4)
SOURCES += tst_qhelpcontentmodel.cpp

CONFIG += help

DEFINES += QT_USE_USING_NAMESPACE
!contains(QT_BUILD_PARTS, tools): DEFINES += QT_NO_BUILD_TOOLS

wince*: {   
   DEFINES += SRCDIR=\\\"./\\\"
   QT += network
   addFiles.sources = $$PWD/data/*.*                
   addFiles.path = data
   clucene.sources = $$QT_BUILD_TREE/lib/QtCLucene*.dll

   DEPLOYMENT += addFiles
   DEPLOYMENT += clucene
   
   DEPLOYMENT_PLUGIN += qsqlite
} else {
   DEFINES += SRCDIR=\\\"$$PWD\\\"
}