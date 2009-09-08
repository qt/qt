:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::
:: Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
:: All rights reserved.
:: Contact: Nokia Corporation (qt-info@nokia.com)
::
:: This file is part of the tools applications of the Qt Toolkit.
::
:: $QT_BEGIN_LICENSE:LGPL$
:: No Commercial Usage
:: This file contains pre-release code and may not be distributed.
:: You may use this file in accordance with the terms and conditions
:: contained in the Technology Preview License Agreement accompanying
:: this package.
::
:: GNU Lesser General Public License Usage
:: Alternatively, this file may be used under the terms of the GNU Lesser
:: General Public License version 2.1 as published by the Free Software
:: Foundation and appearing in the file LICENSE.LGPL included in the
:: packaging of this file.  Please review the following information to
:: ensure the GNU Lesser General Public License version 2.1 requirements
:: will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
::
:: In addition, as a special exception, Nokia gives you certain additional
:: rights.  These rights are described in the Nokia Qt LGPL Exception
:: version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
::
:: If you have questions regarding the use of this file, please contact
:: Nokia at qt-info@nokia.com.
::
::
::
::
::
::
::
::
:: $QT_END_LICENSE$
::
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
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
  type "%IWMAKE_ROOT%\%IWMAKE_TMP%" | find /V "%IWMAKE_TMP2%" >> "%IWMAKE_ROOT%\tmp_line.txt"
  xcopy /Y /Q /R %IWMAKE_ROOT%\tmp_line.txt %IWMAKE_ROOT%\%IWMAKE_TMP% >> %IWMAKE_LOGFILE%
goto :eof

:END
