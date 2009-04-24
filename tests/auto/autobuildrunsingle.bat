@echo off
REM  runtest

SET QTVERSION=%4

IF %1 == winscw goto winscw:
IF %1 == armv5 goto armv5:
IF %1 == gcce goto gcce:
goto end:

:winscw
IF EXIST \epoc32\release\winscw\udeb\tst_%2.exe (
	call \epoc32\release\winscw\udeb\tst_%2.exe -lightxml
	if ERRORLEVEL 1 (		
		echo ^<TestFunction name="PANIC"^> >> \Epoc32\winscw\c\system\data\out.txt
		echo ^<Incident type="fail" file="" line="0"^> >> \Epoc32\winscw\c\system\data\out.txt
		echo ^<DataTag^>^<![CDATA[ message ]]^>^</DataTag^> >> \Epoc32\winscw\c\system\data\out.txt
		echo ^<Description^>^<![CDATA['MESSAGE: TEST CASE PANICS]]^>^</Description^> >> \Epoc32\winscw\c\system\data\out.txt
		echo ^</Incident^> >> \Epoc32\winscw\c\system\data\out.txt
		echo ^</TestFunction^> >> \Epoc32\winscw\c\system\data\out.txt )
	copy /Y \Epoc32\winscw\c\system\data\out.txt %3\%2.xml 
	goto end:
) ELSE (
goto notExist:
)

:ARMV5
IF EXIST \epoc32\release\armv5\urel\tst_%2.exe (
    pushd .
    cd %2
    IF _%5 == _cetest-subdir: (
        echo Running cetest from subdir: %6
        cd %6
    )
    IF _%5 == _cetest-pro: (
    	call cetest -release -f %6 -project-delete -lightxml -o %3\%2.xml
    ) ELSE (
    	call cetest -release -project-delete -lightxml -o %3\%2.xml
    )
    if EXIST %3\%2.xml if ERRORLEVEL 1 (
		echo ^<TestFunction name="PANIC"^> >> %3\%2.xml
		echo ^<Incident type="fail" file="" line="0"^> >> %3\%2.xml
		echo ^<DataTag^>^<![CDATA[ message ]]^>^</DataTag^> >> %3\%2.xml
		echo ^<Description^>^<![CDATA['MESSAGE: TEST CASE PANICS]]^>^</Description^> >> %3\%2.xml
		echo ^</Incident^> >> %3\%2.xml
		echo ^</TestFunction^> >> %3\%2.xml
		)
    popd
	goto end:
) ELSE (
goto notExist:
)

:gcce
goto end:

:notExist
echo AAA: %3\%2.xml
echo ^<Environment^> >> %3\%2.xml
echo ^<QtVersion^>%QTVERSION%^</QtVersion^> >> %3\%2.xml
echo ^<QTestVersion^>%QTVERSION%^</QTestVersion^> >> %3\%2.xml
echo ^</Environment^> >> %3\%2.xml
echo ^<TestFunction name="initTestCase"^> >> %3\%2.xml
echo ^<Incident type="fail" file="" line="0"^> >> %3\%2.xml
echo ^<DataTag^>^<![CDATA[ message ]]^>^</DataTag^> >> %3\%2.xml
echo ^<Description^>^<![CDATA['MESSAGE: COMPILE FAIL]]^>^</Description^> >> %3\%2.xml
echo ^</Incident^> >> %3\%2.xml
echo ^</TestFunction^> >> %3\%2.xml
goto end:

:end
