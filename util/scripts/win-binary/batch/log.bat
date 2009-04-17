call :%1 %2
goto END

:file
set IWMAKE_LOGFILE=%IWMAKE_ROOT%\%~1
call :reset
goto :eof

:fileAbs
set IWMAKE_LOGFILE=%1
call :reset
goto :eof

:reset
date /T > %IWMAKE_LOGFILE%
goto :eof

:END
