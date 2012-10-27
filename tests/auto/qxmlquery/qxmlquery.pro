load(qttest_p4)
SOURCES += tst_qxmlquery.cpp MessageValidator.cpp TestFundament.cpp
HEADERS += PushBaseliner.h                              \
           MessageSilencer.h                            \
           ../qsimplexmlnodemodel/TestSimpleNodeModel.h \
           MessageValidator.h                           \
           NetworkOverrider.h

RESOURCES = input.qrc

QT += network

wince* {
    DEFINES += SRCDIR=\\\"./\\\"
} else:!symbian {
    DEFINES += SRCDIR=\\\"$$PWD/\\\"
}

include (../xmlpatterns.pri)

wince*|symbian: {
   addFiles.files = pushBaselines input.xml
   addFiles.path    = .

   patternistFiles.files = ../xmlpatterns/queries
   symbian: {
       #../xmlpatterns resolves to an illegal path for deployment
       patternistFiles.path    = xmlpatterns
   } else {
       patternistFiles.path    = ../xmlpatterns
   }

   DEPLOYMENT += addFiles patternistFiles
}

CONFIG+=insignificant_test # QTQAINFRA-574
