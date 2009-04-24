@echo off
REM Compile all test cases from given gategory file

FOR /F "eol=# tokens=1*" %%i in (%1) do CALL compilesingle.bat %%i %2
