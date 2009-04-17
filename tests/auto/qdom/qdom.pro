load(qttest_p4)
SOURCES  += tst_qdom.cpp

QT = core xml
QT -= gui

wince*: {
   addFiles.sources = testdata doubleNamespaces.xml umlaut.xml
   addFiles.path = .
   DEPLOYMENT += addFiles

   DEPLOYMENT_PLUGIN += qcncodecs qjpcodecs qkrcodecs qtwcodecs
}
