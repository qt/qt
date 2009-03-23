load(qttest_p4)

SOURCES += ../tst_qprocess.cpp
!wince*: {
TARGET = ../tst_qprocess

win32: {
  CONFIG(debug, debug|release) {
    TARGET = ../../debug/tst_qprocess
} else {
    TARGET = ../../release/tst_qprocess
  }
}
} else {
   TARGET = tst_qprocess
}



QT = core
QT += network
embedded: QT += gui

wince*: {
   addFiles.sources = \
		../fileWriterProcess \
		"../test Space In Name" \
		../testBatFiles \
		../testDetached \
		../testExitCodes \
		../testGuiProcess \
		../testProcessCrash \
		../testProcessDeadWhileReading \
		../testProcessEcho \
		../testProcessEcho2 \
		../testProcessEcho3 \
		../testProcessEchoGui \
		../testProcessEOF \
		../testProcessLoopback \
		../testProcessNormal \
		../testProcessOutput \
		../testProcessSpacesArgs \
		../testSoftExit \
		../testSpaceInName

   addFiles.path = \Program Files\tst_qprocess
   DEPLOYMENT += addFiles
}

