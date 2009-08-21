@echo off

set installsigned_old=%installsigned%
set pkgfile_old=%pkgfile%
set basename_old=%basename%
set signsis1_old=%signsis1%
set signsis2_old=%signsis2%
set signsis3_old=%signsis3%
set unsigned_sis_name_old=%unsigned_sis_name%
set signed_sis_name_old=%signed_sis_name%
set scriptpath_old=%scriptpath%
set certificate_old=%certificate%

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
    set installsigned=true
    set pkgfile=%2
    set basename=%~n2
    set signsis1=%3
    set signsis2=%4
    set signsis3=%5
) else (
    set installsigned=false
    set pkgfile=%1
    set basename=%~n1
    set signsis1=%2
    set signsis2=%3
    set signsis3=%4
)

set unsigned_sis_name=%basename%_unsigned.sis
set signed_sis_name=%basename%.sis

rem Get absolute path to this script
set scriptpath=%~dp0

rem Check the .pkg actually exists.
if not exist %pkgfile% (
    echo Error: Package description file '%pkgfile%' does not exist.
    goto done
)

rem Remove any existing .sis packages
if exist %signed_sis_name% del %signed_sis_name%
if exist %unsigned_sis_name% del %unsigned_sis_name%

rem Create .sis package
makesis %pkgfile% %unsigned_sis_name%

rem If no certificate is given, check default options
if x%signsis1% == x (
    rem If RnD certificate is not found, sign with self signed certificate
    if not exist %scriptpath%..\rd.cer (
        set certificate=Self signed
        signsis %unsigned_sis_name% %signed_sis_name% %scriptpath%..\selfsigned.cer %scriptpath%..\selfsigned.key
        goto install
    )

    rem Sign with RnD certificate
    set certificate=RnD
    signsis %unsigned_sis_name% %signed_sis_name% %scriptpath%..\rd.cer %scriptpath%..\rd-key.pem
) else (
    if x%signsis2% == x (
       echo Custom certificate key file parameter missing.
       goto cleanup
    )

    set certificate=%signsis1%
    signsis %unsigned_sis_name% %signed_sis_name% %signsis1% %signsis2% %signsis3%
)

:install
if exist %signed_sis_name% (
    echo Successfully created %signed_sis_name% using certificate %certificate%
    if "%installsigned%" == "true" (
        echo Installing %signed_sis_name%...
        call %signed_sis_name%
    )
)

:cleanup
if exist %unsigned_sis_name% del %unsigned_sis_name%

:done

set installsigned=%installsigned_old%
set pkgfile=%pkgfile_old%
set basename=%basename_old%
set signsis1=%signsis1_old%
set signsis2=%signsis2_old%
set signsis3=%signsis3_old%
set unsigned_sis_name=%unsigned_sis_name_old%
set signed_sis_name=%signed_sis_name_old%
set scriptpath=%scriptpath_old%
set certificate=%certificate_old%
