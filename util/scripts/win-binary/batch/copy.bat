call :%1 %2
goto END

:dest
  set IWMAKE_OUTDIR=%IWMAKE_ROOT%\%~1
  if not exist %IWMAKE_OUTDIR% mkdir %IWMAKE_OUTDIR%
goto :eof

:src
  set IWMAKE_SRCDIR=%IWMAKE_ROOT%\%~1
  if not exist %IWMAKE_SRCDIR% mkdir %IWMAKE_SRCDIR%
goto :eof

:destAbs
  set IWMAKE_OUTDIR=%1
  if not exist %IWMAKE_OUTDIR% mkdir %IWMAKE_OUTDIR%
goto :eof

:srcAbs
  set IWMAKE_SRCDIR=%1
  if not exist %IWMAKE_SRCDIR% mkdir %IWMAKE_SRCDIR%
goto :eof

:extsync
  echo Copying %IWMAKE_EXTERNAL%\%~1 to %IWMAKE_ROOT%\%~1 >> %IWMAKE_LOGFILE%
  if exist %IWMAKE_ROOT%\%~1 rd /S /Q %IWMAKE_ROOT%\%~1 >> %IWMAKE_LOGFILE%
  xcopy /H /Y /Q /I /R /E %IWMAKE_EXTERNAL%\%~1 %IWMAKE_ROOT%\%~1 >> %IWMAKE_LOGFILE%
goto :eof

:all
  xcopy /H /Y /Q /I /R /S %IWMAKE_SRCDIR%\%~1 %IWMAKE_OUTDIR%\ >> %IWMAKE_LOGFILE%
goto :eof

:filesEx
  for /F "tokens=1,2*" %%m in ("%~1") do set IWMAKE_TMP=%%~m& set IWMAKE_TMP2=%%~n& set IWMAKE_TMP3=%%~o
  echo %IWMAKE_TMP3% > %IWMAKE_ROOT%\iwmake_exclude
  xcopy /H /Y /Q /I /R /S /EXCLUDE:%IWMAKE_ROOT%\iwmake_exclude %IWMAKE_SRCDIR%\%IWMAKE_TMP% %IWMAKE_OUTDIR%\%IWMAKE_TMP2% >> %IWMAKE_LOGFILE%
goto :eof

:file
  set IWMAKE_TMP=%~1
  set IWMAKE_TMP2=%~nx1
  echo set IWMAKE_TMP3=%%IWMAKE_TMP:%IWMAKE_TMP2%=%%>"%IWMAKE_ROOT%\iwmake_tmp.bat"
  call %IWMAKE_ROOT%\iwmake_tmp.bat
  if not exist "%IWMAKE_OUTDIR%\%IWMAKE_TMP3%" mkdir "%IWMAKE_OUTDIR%\%IWMAKE_TMP3%"
  xcopy /H /Y /Q /I /R %IWMAKE_SRCDIR%\%IWMAKE_TMP% %IWMAKE_OUTDIR%\%IWMAKE_TMP3% >> %IWMAKE_LOGFILE%
goto :eof

:fileAndRename
  for /F "tokens=1*" %%m in ("%~1") do set IWMAKE_TMP=%%~m& set IWMAKE_TMP2=%%~n& set IWMAKE_TMP4=%%~nxn
  echo set IWMAKE_TMP3=%%IWMAKE_TMP2:%IWMAKE_TMP4%=%%>"%IWMAKE_ROOT%\iwmake_tmp.bat"
  call %IWMAKE_ROOT%\iwmake_tmp.bat
  if not exist "%IWMAKE_OUTDIR%\%IWMAKE_TMP3%" mkdir "%IWMAKE_OUTDIR%\%IWMAKE_TMP3%"
  echo > %IWMAKE_OUTDIR%\%IWMAKE_TMP2%
  xcopy /H /Y /Q /R %IWMAKE_SRCDIR%\%IWMAKE_TMP% %IWMAKE_OUTDIR%\%IWMAKE_TMP2% >> %IWMAKE_LOGFILE%
  set IWMAKE_TMP4=
goto :eof

:files
  for /F "tokens=1*" %%m in ("%~1") do set IWMAKE_TMP=%%~m& set IWMAKE_TMP2=%%~n
  if not exist "%IWMAKE_OUTDIR%\%IWMAKE_TMP2%" mkdir "%IWMAKE_OUTDIR%\%IWMAKE_TMP2%"
  xcopy /H /Y /Q /I /R /S %IWMAKE_SRCDIR%\%IWMAKE_TMP% %IWMAKE_OUTDIR%\%IWMAKE_TMP2% >> %IWMAKE_LOGFILE%
goto :eof

:runtime
  for /F "tokens=1*" %%m in ("%~1") do set IWMAKE_TMP=%%~m& set IWMAKE_TMP2=%%~n
  if "%IWMAKE_TMP2%" == "" set IWMAKE_TMP2=bin
  if "%IWMAKE_TMP%" == "mingw" (
    xcopy /H /Y /Q /I /R %IWMAKE_MINGWPATH%\bin\mingwm*.dll %IWMAKE_OUTDIR%\%IWMAKE_TMP2%\ >> %IWMAKE_LOGFILE%
  ) else (
    xcopy /H /Y /Q /I /R %SystemRoot%\system32\msvcr%IWMAKE_TMP%.dll %IWMAKE_OUTDIR%\%IWMAKE_TMP2%\ >> %IWMAKE_LOGFILE%
    xcopy /H /Y /Q /I /R %SystemRoot%\system32\msvcp%IWMAKE_TMP%.dll %IWMAKE_OUTDIR%\%IWMAKE_TMP2%\ >> %IWMAKE_LOGFILE%
  )
goto :eof

:external
  for /F "tokens=1*" %%m in ("%~1") do set IWMAKE_TMP=%%~m& set IWMAKE_TMP2=%%~n
  xcopy /H /Y /Q /I /R %IWMAKE_TMP% %IWMAKE_OUTDIR%\%IWMAKE_TMP2%\ >> %IWMAKE_LOGFILE%
goto :eof

:syncqt
  pushd %IWMAKE_OUTDIR%
  if exist "include" rd /S /Q include
  if not "%QTDIR%"=="" set IWMAKE_OLD_QTDIR=%QTDIR%
  set QTDIR=%IWMAKE_OUTDIR%
  "%IWMAKE_PERLPATH%\perl" %QTDIR%\bin\syncqt -copy >> %IWMAKE_LOGFILE% 2>&1
  if not "%IWMAKE_OLD_QTDIR%"=="" set QTDIR=%IWMAKE_OLD_QTDIR%
  popd
goto :eof

:END
