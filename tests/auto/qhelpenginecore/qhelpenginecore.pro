load(qttest_p4)
SOURCES += tst_qhelpenginecore.cpp
CONFIG  += help
QT      += sql


DEFINES += QT_USE_USING_NAMESPACE
!contains(QT_BUILD_PARTS, tools): DEFINES += QT_NO_BUILD_TOOLS

wince*: {   
   DEFINES += SRCDIR=\\\"./\\\"
   QT += network
   addFiles.files = $$PWD/data/*.*                
   addFiles.path = data
   clucene.files = $$QT_BUILD_TREE/lib/QtCLucene*.dll

   DEPLOYMENT += addFiles
   DEPLOYMENT += clucene

   DEPLOYMENT_PLUGIN += qsqlite
} else {
   DEFINES += SRCDIR=\\\"$$PWD\\\"
}
