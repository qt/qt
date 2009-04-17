TEMPLATE = subdirs
SUBDIRS = testProcessCrash \
          testProcessEcho \
          testProcessEcho2 \
          testProcessEcho3 \
          testProcessLoopback \
          testProcessNormal \
          testProcessOutput \
          testProcessDeadWhileReading \
          testProcessEOF \
          testSoftExit \
          testProcessSpacesArgs/nospace.pro \
          testProcessSpacesArgs/onespace.pro \
          testProcessSpacesArgs/twospaces.pro \
          testExitCodes \
          testSpaceInName \
          testGuiProcess \
          testDetached \
          fileWriterProcess \
          testSetWorkingDirectory


win32:!wince*:SUBDIRS+=testProcessEchoGui

SUBDIRS += test


