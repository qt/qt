load(qttest_p4)
SOURCES         += tst_qitemmodel.cpp

QT += sql

symbian:TARGET.EPOCHEAPSIZE="0x100000 0x1000000 // Min 1Mb, max 16Mb"

# NOTE: The deployment of the sqldrivers is disabled on purpose.
#       If we deploy the plugins, they are loaded twice when running
#       the tests on the autotest system. In that case we run out of
#       memory on Windows Mobile 5.

#wince*: {
#   plugFiles.sources = $$QT_BUILD_TREE/plugins/sqldrivers/*.dll
#   plugFiles.path    = sqldrivers
#   DEPLOYMENT += plugFiles 
#}
