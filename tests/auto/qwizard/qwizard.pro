load(qttest_p4)
SOURCES  += tst_qwizard.cpp tst_qwizard_2.cpp
#SOURCES  += /home/jasmin/dev/solutions/widgets/qtwizard/src/qtwizard.cpp
#HEADERS  += /home/jasmin/dev/solutions/widgets/qtwizard/src/qtwizard.h
#SOURCES  += /home/jasplin/dev/research/qwizard/src/qwizard.cpp
#HEADERS  += /home/jasplin/dev/research/qwizard/src/qwizard.h
RESOURCES = qwizard.qrc
TARGET.EPOCHEAPSIZE = 0x200000 0xF00000

CONFIG+=insignificant_test # QTQAINFRA-428
