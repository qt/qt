@echo off
REM Compile all test cases from given category file

REM Create root directory for autotest results
IF NOT EXIST %3 MKDIR %3

REM Set Module variable
FOR /F %%i IN ("%1") DO SET MODULE=%%~ni

REM run single test
FOR /F "eol=# tokens=1*" %%i in (%1) do CALL autobuildrunsingle.bat %2 %%i %3 %4 %%j %%k
