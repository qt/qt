@echo off
IF "%1" EQU "wincewm50pocket-msvc2005" (
checksdk.exe -sdk "Windows Mobile 5.0 Pocket PC SDK (ARMV4I)" -script tmp_created_script_setup.bat 1>NUL
tmp_created_script_setup.bat
del tmp_created_script_setup.bat
echo Windows Mobile 5.01 for Pocket PC selected, environment is set up
) ELSE IF "%1" EQU "wincewm50smart-msvc2005" (
checksdk.exe -sdk "Windows Mobile 5.0 Smartphone SDK (ARMV4I)" -script tmp_created_script_setup.bat 1>NUL
tmp_created_script_setup.bat
del tmp_created_script_setup.bat
echo Windows Mobile 5.01 for Smartphone for arm selected, environment is set up
) ELSE IF "%1" EQU "wince50standard-x86-msvc2005" (
checksdk.exe -sdk "STANDARDSDK_500 (x86)" -script tmp_created_script_setup.bat 1>NUL
tmp_created_script_setup.bat
del tmp_created_script_setup.bat
echo Standard SDK selected, environment is set up
) ELSE IF "%1" EQU "wince50standard-armv4i-msvc2005" (
checksdk.exe -sdk "STANDARDSDK_500 (ARMV4I)" -script tmp_created_script_setup.bat 1>NUL
tmp_created_script_setup.bat
del tmp_created_script_setup.bat
echo Standard SDK for arm selected, environment is set up
) ELSE IF "%1" EQU "wince50standard-mipsii-msvc2005" (
checksdk.exe -sdk "STANDARDSDK_500 (MIPSII)" -script tmp_created_script_setup.bat 1>NUL
tmp_created_script_setup.bat
del tmp_created_script_setup.bat
echo Standard SDK for mips-ii selected, environment is set up
) ELSE IF "%1" EQU "wince50standard-mipsiv-msvc2005" (
checksdk.exe -sdk "STANDARDSDK_500 (MIPSIV)" -script tmp_created_script_setup.bat 1>NUL
tmp_created_script_setup.bat
del tmp_created_script_setup.bat
echo Standard SDK for mips-iv selected, environment is set up
) ELSE IF "%1" EQU "wince50standard-sh4-msvc2005" (
checksdk.exe -sdk "STANDARDSDK_500 (SH4)" -script tmp_created_script_setup.bat 1>NUL
tmp_created_script_setup.bat
del tmp_created_script_setup.bat
echo Standard SDK for sh4 selected, environment is set up
) ELSE IF "%1" EQU "wincewm60professional-msvc2005" (
checksdk.exe -sdk "Windows Mobile 6 Professional SDK (ARMV4I)" -script tmp_created_script_setup.bat 1>NUL
tmp_created_script_setup.bat
del tmp_created_script_setup.bat
echo Windows Mobile 6 Professional selected, environment is set up
) ELSE IF "%1" EQU "wincewm60standard-msvc2005" (
checksdk.exe -sdk "Windows Mobile 6 Standard SDK (ARMV4I)" -script tmp_created_script_setup.bat 1>NUL
tmp_created_script_setup.bat
del tmp_created_script_setup.bat
echo Windows Mobile 6 Standard selected, environment is set up
) ELSE IF "%1" EQU "wincewm50pocket-msvc2008" (
checksdk.exe -sdk "Windows Mobile 5.0 Pocket PC SDK (ARMV4I)" -script tmp_created_script_setup.bat 1>NUL
tmp_created_script_setup.bat
del tmp_created_script_setup.bat
echo Windows Mobile 5.01 for Pocket PC selected, environment is set up
) ELSE IF "%1" EQU "wincewm50smart-msvc2008" (
checksdk.exe -sdk "Windows Mobile 5.0 Smartphone SDK (ARMV4I)" -script tmp_created_script_setup.bat 1>NUL
tmp_created_script_setup.bat
del tmp_created_script_setup.bat
echo Windows Mobile 5.01 for Smartphone for arm selected, environment is set up
) ELSE IF "%1" EQU "wince50standard-x86-msvc2008" (
checksdk.exe -sdk "STANDARDSDK_500 (x86)" -script tmp_created_script_setup.bat 1>NUL
tmp_created_script_setup.bat
del tmp_created_script_setup.bat
echo Standard SDK selected, environment is set up
) ELSE IF "%1" EQU "wince50standard-armv4i-msvc2008" (
checksdk.exe -sdk "STANDARDSDK_500 (ARMV4I)" -script tmp_created_script_setup.bat 1>NUL
tmp_created_script_setup.bat
del tmp_created_script_setup.bat
echo Standard SDK for arm selected, environment is set up
) ELSE IF "%1" EQU "wince50standard-mipsii-msvc2008" (
checksdk.exe -sdk "STANDARDSDK_500 (MIPSII)" -script tmp_created_script_setup.bat 1>NUL
tmp_created_script_setup.bat
del tmp_created_script_setup.bat
echo Standard SDK for mips-ii selected, environment is set up
) ELSE IF "%1" EQU "wince50standard-mipsiv-msvc2008" (
checksdk.exe -sdk "STANDARDSDK_500 (MIPSIV)" -script tmp_created_script_setup.bat 1>NUL
tmp_created_script_setup.bat
del tmp_created_script_setup.bat
echo Standard SDK for mips-iv selected, environment is set up
) ELSE IF "%1" EQU "wince50standard-sh4-msvc2008" (
checksdk.exe -sdk "STANDARDSDK_500 (SH4)" -script tmp_created_script_setup.bat 1>NUL
tmp_created_script_setup.bat
del tmp_created_script_setup.bat
echo Standard SDK for sh4 selected, environment is set up
) ELSE IF "%1" EQU "wincewm60professional-msvc2008" (
checksdk.exe -sdk "Windows Mobile 6 Professional SDK (ARMV4I)" -script tmp_created_script_setup.bat 1>NUL
tmp_created_script_setup.bat
del tmp_created_script_setup.bat
echo Windows Mobile 6 Professional selected, environment is set up
) ELSE IF "%1" EQU "wincewm60standard-msvc2008" (
checksdk.exe -sdk "Windows Mobile 6 Standard SDK (ARMV4I)" -script tmp_created_script_setup.bat 1>NUL
tmp_created_script_setup.bat
del tmp_created_script_setup.bat
echo Windows Mobile 6 Standard selected, environment is set up
) ELSE (
echo no SDK to build Windows CE selected
echo.
echo Current choices are:
echo   wincewm50pocket-msvc2005        - SDK for Windows Mobile 5.01 PocketPC
echo   wincewm50smart-msvc2005         - SDK for Windows Mobile 5.01 Smartphone
echo   wince50standard-x86-msvc2005    - Build for the WinCE standard SDK 5.0
echo                                     with x86 platform preset
echo   wince50standard-armv4i-msvc2005 - Build for the WinCE standard SDK 5.0
echo                                     with armv4i platform preset
echo   wince50standard-mipsiv-msvc2005 - Build for the WinCE standard SDK 5.0
echo                                     with mips platform preset
echo   wince50standard-sh4-msvc2005    - Build for the WinCE standard SDK 5.0
echo                                     with sh4 platform preset
echo   wincewm60professional-msvc2005  - SDK for Windows Mobile 6 professional
echo   wincewm60standard-msvc2005      - SDK for Windows Mobile 6 Standard
echo and the corresponding versions for msvc2008.
echo.
)



