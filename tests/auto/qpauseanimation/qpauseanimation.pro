load(qttest_p4)
QT = core gui
SOURCES  += tst_qpauseanimation.cpp

win32:CONFIG+=insignificant_test # QTBUG-28069
