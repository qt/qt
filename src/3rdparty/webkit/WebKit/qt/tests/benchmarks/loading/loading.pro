isEmpty(OUTPUT_DIR): OUTPUT_DIR = ../../../../..
exists($${TARGET}.qrc):RESOURCES += $${TARGET}.qrc
include(../../tests.pri)
