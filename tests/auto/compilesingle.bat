@echo off
REM Compile test case from folder as a parameter

pushd .
cd %1
REM call abld reallyclean
call qmake -r
call bldmake bldfiles

REM reallyclean is not always deleting executables for some reason, so do that explicitly before building

IF %2 == winscw (
call abld reallyclean winscw
call del /q /f \epoc32\release\winscw\udeb\tst_%1.exe
call abld build winscw udeb
)
IF %2 == armv5 (
call abld reallyclean armv5
call del /q /f \epoc32\release\armv5\urel\tst_%1.exe
call abld build armv5 urel
)
IF %2 == gcce (
call abld reallyclean gcce
call del /q /f \epoc32\release\gcce\urel\tst_%1.exe
call abld build gcce urel
)

popd