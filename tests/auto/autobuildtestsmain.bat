@echo off
SET QTVERSION=4.4.2

IF NOT EXIST %2\result\sent MKDIR %2\result\sent

REM Delete old results
del /Q %2\*.*

REM Build enviroment
set path=%cd%\bin;%path%
make confclean
configure -cetest -platform win32-mwc -xplatform symbian-abld -openssl-linked -qconfig symbian
call bldmake bldfiles
call abld clean 
call abld reallyclean
IF %1 == winscw call abld build winscw udeb
IF %1 == armv5 call abld build armv5 urel
IF %1 == gcce call abld build gcce urel

REM Build auto tests
cd \qts60\tests\auto
for %%i in (QtCore QtGui QtNetwork QtXml QtSvg QtScript) do call compilecategory.bat _Categories\%%i.txt %1


REM run auto tests
cd \qts60\tests\auto
for %%i in (QtCore QtGui QtNetwork QtXml QtSvg QtScript) do call autobuildruncategory.bat _Categories\%%i.txt %1 %2 %QTVERSION%

REM Collect test results
cd \qts60\tests\auto
echo call python joinresults.py %1 %2 %QTVERSION%
call python joinresults.py %1 %2 %QTVERSION%

REM send the result file
call %3pscp.exe -i "%USERPROFILE%\.ssh\puttyssh.ppk" %2\Result\*.xml qtestmaster@kramer-nokia.troll.no:/home/qtestmaster/results

REM Copy the result file to different directory that it not sent again
move %2\Result\*.xml %2\Result\sent
