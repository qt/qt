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

!symbian: {
    QT += network
}

embedded: QT += gui

wince*: {
       
   addFile_fileWriterProcess.sources = $$OUT_PWD/../fileWriterProcess/fileWriterProcess.exe 
   addFile_fileWriterProcess.path = fileWriterProcess
  
   addFile_testBatFiles.sources = $$PWD/../testBatFiles/*
   addFile_testBatFiles.path = testBatFiles
   
   addFile_testDetached.sources = $$OUT_PWD/../testDetached/testDetached.exe
   addFile_testDetached.path = testDetached
   
   addFile_testExitCodes.sources = $$OUT_PWD/../testExitCodes/testExitCodes.exe
   addFile_testExitCodes.path = testExitCodes
   
   addFile_testGuiProcess.sources = $$OUT_PWD/../testGuiProcess/testGuiProcess.exe
   addFile_testGuiProcess.path = testGuiProcess

   addFile_testProcessCrash.sources = $$OUT_PWD/../testProcessCrash/testProcessCrash.exe
   addFile_testProcessCrash.path = testProcessCrash
   
   addFile_testProcessDeadWhileReading.sources = $$OUT_PWD/../testProcessDeadWhileReading/testProcessDeadWhileReading.exe
   addFile_testProcessDeadWhileReading.path = testProcessDeadWhileReading
   
   addFile_testProcessEcho.sources = $$OUT_PWD/../testProcessEcho/testProcessEcho.exe
   addFile_testProcessEcho.path = testProcessEcho
   
   addFile_testProcessEcho2.sources = $$OUT_PWD/../testProcessEcho2/testProcessEcho2.exe 
   addFile_testProcessEcho2.path = testProcessEcho2
   
   addFile_testProcessEcho3.sources = $$OUT_PWD/../testProcessEcho3/testProcessEcho3.exe
   addFile_testProcessEcho3.path = testProcessEcho3
   
   addFile_testProcessEOF.sources = $$OUT_PWD/../testProcessEOF/testProcessEOF.exe 
   addFile_testProcessEOF.path = testProcessEOF
   
   addFile_testProcessLoopback.sources = $$OUT_PWD/../testProcessLoopback/testProcessLoopback.exe
   addFile_testProcessLoopback.path = testProcessLoopback
   
   addFile_testProcessNormal.sources = $$OUT_PWD/../testProcessNormal/testProcessNormal.exe 
   addFile_testProcessNormal.path = testProcessNormal
   
   addFile_testProcessOutput.sources = $$OUT_PWD/../testProcessOutput/testProcessOutput.exe
   addFile_testProcessOutput.path = testProcessOutput
   
   addFile_testProcessNoSpacesArgs.sources = $$OUT_PWD/../testProcessSpacesArgs/nospace.exe
   addFile_testProcessNoSpacesArgs.path = testProcessSpacesArgs
   
   addFile_testProcessOneSpacesArgs.sources = $$OUT_PWD/../testProcessSpacesArgs/"one space".exe
   addFile_testProcessOneSpacesArgs.path = testProcessSpacesArgs
   
   addFile_testProcessTwoSpacesArgs.sources = $$OUT_PWD/../testProcessSpacesArgs/"two space s".exe
   addFile_testProcessTwoSpacesArgs.path = testProcessSpacesArgs
   
   addFile_testSoftExit.sources = $$OUT_PWD/../testSoftExit/testSoftExit.exe
   addFile_testSoftExit.path = testSoftExit
   
   addFile_testSpaceInName.sources = $$OUT_PWD/../"test Space In Name"/testSpaceInName.exe
   addFile_testSpaceInName.path = "test Space In Name"
   
   
   DEPLOYMENT += addFile_fileWriterProcess \
                 addFile_testBatFiles \
                 addFile_testDetached \
                 addFile_testExitCodes  \
                 addFile_testGuiProcess \
                 addFile_testProcessCrash \
                 addFile_testProcessDeadWhileReading \
                 addFile_testProcessEcho \
                 addFile_testProcessEcho2 \
                 addFile_testProcessEcho3 \
                 addFile_testProcessEchoGui \
                 addFile_testProcessEOF \
                 addFile_testProcessLoopback \
                 addFile_testProcessNormal \
                 addFile_testProcessOutput \
                 addFile_testProcessNoSpacesArgs \
                 addFile_testProcessOneSpacesArgs \
                 addFile_testProcessTwoSpacesArgs \
                 addFile_testSoftExit \
                 addFile_testSpaceInName 
}

symbian: {
   binDep.sources = \
        fileWriterProcess.exe \
        testDetached.exe \
        testExitCodes.exe \
        testProcessCrash.exe \
        testProcessEcho.exe \
        testProcessNormal.exe \
        testProcessOutput.exe \
        nospace.exe \
        testSpaceInName.exe
   binDep.path = \\sys\\bin

   DEPLOYMENT += binDep
}
