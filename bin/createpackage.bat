@echo off

rem Help text
if "%1"=="" (
    echo Convenience script for creating signed packages you can install on your phone.
    echo Usage: createpackage.bat [-i] myexample_armv5_udeb.pkg [certificate key [passphrase]]
    echo.
    echo If no certificate and key files are provided, either a RnD certificate or
    echo a self-signed certificate from Qt installation root directory is used.
    echo.
    echo To install the package right away using PC suite, use -i argument. 
    goto done
)

if "%1"=="-i" (
    set installsisx=true
    set pkgfile=%2
    set basename=%~n2
    set signsis1=%3
    set signsis2=%4
    set signsis3=%5
) else (
    set installsisx=false
    set pkgfile=%1
    set basename=%~n1
    set signsis1=%2
    set signsis2=%3
    set signsis3=%4
)

rem Check the .pkg actually exists.
if not exist %pkgfile% (
    echo Error: Package description file '%pkgfile%' does not exist.
    goto done
)

rem Get absolute path to this script
set scriptpath=%~dp0

rem Remove any existing .sis or .sisx packages
if exist %basename%.sis del %basename%.sis
if exist %basename%.sisx del %basename%.sisx

rem Create .sis package
makesis %pkgfile%

rem If no certificate is given, check default options
if x%signsis1% == x (
    rem If RnD certificate is not found, sign with self signed certificate
    if not exist %scriptpath%..\rd.cer (
        set certificate=Self signed
        signsis %basename%.sis %basename%.sisx %scriptpath%..\selfsigned.cer %scriptpath%..\selfsigned.key
        goto install
    )

    rem Sign with RnD certificate
    set certificate=RnD
    signsis %basename%.sis %basename%.sisx %scriptpath%..\rd.cer %scriptpath%..\rd-key.pem
) else (
    if x%signsis2% == x (
       echo Custom certificate key file parameter missing.
       goto cleanup
    )

    set certificate=%signsis1%
    signsis %basename%.sis %basename%.sisx %signsis1% %signsis2% %signsis3%
)

:install
if exist %basename%.sisx (
    echo Successfully created %basename%.sisx using certificate %certificate%
    if "%installsisx%" == "true" (
        echo Installing %basename%.sisx...
        call %basename%.sisx
    )
)

:cleanup
if exist %basename%.sis del %basename%.sis

:done