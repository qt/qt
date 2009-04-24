@echo off
REM NOTE: If this script is modified, it is likely that next automatic test will fail horribly,
REM       unless manual pull from repository is done before running it.


REM Sync git
cd ..\..\
call git clean -dfx
//call git reset --hard
call git pull


REM Auto tests builder
REM 1. argument is enviroment: wincw, armv5 or gcce
REM 2. argument is result's path: like \autotestResults
REM 3. argument is path to pscp.exe: like c:\

call tests\auto\autobuildtestsmain.bat %1 %2 %3

