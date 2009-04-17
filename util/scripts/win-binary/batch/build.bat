call :%1 %2
goto END

:begin
for /F "tokens=1*" %%m in ("%~1") do set IWMAKE_COMPILER=%%~m& set IWMAKE_TMP=%%~n

if "%IWMAKE_TMP%"=="" set IWMAKE_TMP=build_%IWMAKE_COMPILER%
set IWMAKE_BUILD=%IWMAKE_ROOT%\%IWMAKE_TMP%
if not exist %IWMAKE_BUILD% mkdir %IWMAKE_BUILD%

set IWMAKE_OLD_PATH=%PATH%
if not "%QMAKESPEC%"=="" set IWMAKE_OLD_QMAKESPEC=%QMAKESPEC%
if not "%QTDIR%"=="" set IWMAKE_OLD_QTDIR=%QTDIR%
if not "%INCLUDE%"=="" set IWMAKE_OLD_INCLUDE=%INCLUDE%
if not "%LIB%"=="" set IWMAKE_OLD_LIB=%LIB%

set PATH=%IWMAKE_BUILD%\bin;%PATH%
set QTDIR=%IWMAKE_BUILD%

if "%IWMAKE_COMPILER%"=="vs2003" goto VS2003Env
if "%IWMAKE_COMPILER%"=="vs2002" goto VS2002Env
if "%IWMAKE_COMPILER%"=="vs2005" goto VS2005Env
if "%IWMAKE_COMPILER%"=="vs2008" goto VS2008Env
if "%IWMAKE_COMPILER%"=="vc60" goto VC60Env
if "%IWMAKE_COMPILER%"=="mingw" goto MinGWEnv
goto :eof

:VS2003Env
set QMAKESPEC=win32-msvc2003
set IWMAKE_MAKE=nmake
goto :eof

:VS2002Env
set QMAKESPEC=win32-msvc2002
set IWMAKE_MAKE=nmake
goto :eof

:VS2005Env
set QMAKESPEC=win32-msvc2005
set IWMAKE_MAKE=nmake
goto :eof

:VS2008Env
set QMAKESPEC=win32-msvc2008
set IWMAKE_MAKE=nmake
goto :eof

:VC60Env
set QMAKESPEC=win32-msvc
set IWMAKE_MAKE=nmake
goto :eof

:SetVSEnvironment
if not exist %1 echo %1 not found >> %IWMAKE_LOGFILE% & exit /b 1
call %1 >> %IWMAKE_LOGFILE%
goto :eof

:MinGWEnv
set QMAKESPEC=win32-g++
set IWMAKE_MAKE=mingw32-make
goto :eof

:finish
  if not "%IWMAKE_OLD_PATH%"=="" set PATH=%IWMAKE_OLD_PATH%& set IWMAKE_OLD_PATH=
  if not "%IWMAKE_OLD_QMAKESPEC%"=="" set QMAKESPEC=%IWMAKE_OLD_QMAKESPEC%& set IWMAKE_OLD_QMAKESPEC=
  if not "%IWMAKE_OLD_QTDIR%"=="" set QTDIR=%IWMAKE_OLD_QTDIR%& set IWMAKE_OLD_QTDIR=
  if not "%IWMAKE_OLD_INCLUDE%"=="" set INCLUDE=%IWMAKE_OLD_INCLUDE%& set IWMAKE_OLD_INCLUDE=
  if not "%IWMAKE_OLD_LIB%"=="" set LIB=%IWMAKE_OLD_LIB%& set IWMAKE_OLD_LIB=
goto :eof

:configure
  pushd %IWMAKE_BUILD%
  configure %~1 >> %IWMAKE_LOGFILE% 2>&1
  popd
goto :eof

:shadowconfigure
  for /F "tokens=1*" %%m in ("%~1") do set IWMAKE_TMP=%%~m& set IWMAKE_TMP2=%%~n
  pushd %IWMAKE_BUILD%
  %IWMAKE_ROOT%\%IWMAKE_TMP%\configure %IWMAKE_TMP2% >> %IWMAKE_LOGFILE% 2>&1
  popd
goto :eof

:setcepaths
  call %IWMAKE_BUILD%\bin\setcepaths.bat %~1 >> %IWMAKE_LOGFILE% 2>&1
goto :eof

:bin
  pushd %IWMAKE_BUILD%
  %IWMAKE_MAKE% %~1 >>%IWMAKE_LOGFILE% 2>&1
  popd
goto :eof

:binInDir
  for /F "tokens=1*" %%m in ("%~1") do set IWMAKE_TMP=%%~m& set IWMAKE_TMP2=%%~n
  pushd %IWMAKE_BUILD%\%IWMAKE_TMP%
  %IWMAKE_MAKE% %IWMAKE_TMP2% >> %IWMAKE_LOGFILE% 2>&1
  popd
goto :eof

:DBPlugins
  call "%IWMAKE_SCRIPTDIR%\batch\copy.bat" extsync sql
  set IWMAKE_SQL_OLD_LIB=%LIB%

  set LIB=%IWMAKE_ROOT%\sql\%IWMAKE_COMPILER%;%IWMAKE_SQL_OLD_LIB%
  pushd %IWMAKE_BUILD%\src\plugins\sqldrivers\psql
  qmake "INCLUDEPATH+=%IWMAKE_ROOT%\sql\include\psql" "LIBS+=libpqd.lib ws2_32.lib advapi32.lib shfolder.lib shell32.lib" >> %IWMAKE_LOGFILE% 2>&1
  %IWMAKE_MAKE% debug >> %IWMAKE_LOGFILE% 2>&1
  qmake "INCLUDEPATH+=%IWMAKE_ROOT%\sql\include\psql" "LIBS+=libpq.lib ws2_32.lib advapi32.lib shfolder.lib shell32.lib" >> %IWMAKE_LOGFILE% 2>&1
  %IWMAKE_MAKE% release >> %IWMAKE_LOGFILE% 2>&1
  popd
  set LIB=%IWMAKE_SQL_OLD_LIB%
  set IWMAKE_SQL_OLD_LIB=
goto :eof

:root
  set IWMAKE_BUILD=%~1
  if not exist %IWMAKE_BUILD% mkdir %IWMAKE_BUILD%
goto :eof

:END
