load(qttest_p4)
TARGET.EPOCHEAPSIZE = 0x200000 0x800000
SOURCES  += tst_qtableview.cpp

CONFIG+=insignificant_test # QTQAINFRA-428
