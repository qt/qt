call :%1 %2
goto END

:destDir
  if exist "%IWMAKE_OUTDIR%\%~1" rd /S /Q %IWMAKE_OUTDIR%\%~1
goto :eof

:dir
  if exist "%IWMAKE_ROOT%\%~1" rd /S /Q %IWMAKE_ROOT%\%~1
goto :eof

:dirAbs
  if exist "%~1"  rd /S /Q %~1
goto :eof

:file
  del /Q /F %IWMAKE_OUTDIR%\%~1 >> %IWMAKE_LOGFILE% 2>&1
  exit /b 0
goto :eof

:files
  del /S /Q /F %IWMAKE_OUTDIR%\%~1 >> %IWMAKE_LOGFILE% 2>&1
  exit /b 0
goto :eof

:line
  for /F "tokens=1*" %%m in ("%~1") do set IWMAKE_TMP=%%~m& set IWMAKE_TMP2=%%~n
  if exist "%IWMAKE_ROOT%\tmp_line.txt" del /Q /F "%IWMAKE_ROOT%\tmp_line.txt" >> %IWMAKE_LOGFILE%
  type "%IWMAKE_ROOT%\%IWMAKE_TMP%" | %WINDIR%\system32\find /V "%IWMAKE_TMP2%" >> "%IWMAKE_ROOT%\tmp_line.txt"
  xcopy /Y /Q /R %IWMAKE_ROOT%\tmp_line.txt %IWMAKE_ROOT%\%IWMAKE_TMP% >> %IWMAKE_LOGFILE%
goto :eof

:dirs
  for /R "%IWMAKE_OUTDIR%" %%d in ("%~1") do (
    if exist %%d (
      rd /S /Q %%d >> %IWMAKE_LOGFILE% 2>&1
    )
  )
goto :eof

:END
