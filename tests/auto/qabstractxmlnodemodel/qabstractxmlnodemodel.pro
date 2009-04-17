load(qttest_p4)
SOURCES += tst_qabstractxmlnodemodel.cpp    \
           LoadingModel.cpp                 \
           ../qxmlquery/TestFundament.cpp
HEADERS += TestNodeModel.h LoadingModel.h

include (../xmlpatterns.pri)

wince*: {
   addFiles.sources = tree.xml
   addFiles.path    = .

   DEPLOYMENT += addFiles
}
