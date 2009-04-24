#######################################################################
#
# A script for creating binary release package
#
# Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
# Contact: Qt Software Information (qt-info@nokia.com)
#
#######################################################################

my $tempDir = "_binary_package_dir_";

if (@ARGV)
{
    my $pkgFileName = shift(@ARGV);
    my $epocroot = shift(@ARGV);

    if (!-r($pkgFileName))
    {
        print("Package file doesn't exist!\n");
        exit;
    }

    if ($epocroot eq "")
    {
        $epocroot = "\\";
    }

    my $armDbgDir = "epoc32\\release\\armv5\\udeb";
    my $armRelDir = "epoc32\\release\\armv5\\urel";
    my $gcceDbgDir = "epoc32\\release\\gcce\\udeb";
    my $gcceRelDir = "epoc32\\release\\gcce\\urel";
    my $armLibDir = "epoc32\\release\\armv5\\lib";

    # Clear archive flag from all items in \epoc32\release\armv5\urel and \epoc32\release\armv5\lib
    # as those will have the binaries used for all platforms and builds.
    my $systemCmd = "attrib -A ".$epocroot.$armDbgDir."\\*.*";
    runSystemCmd($systemCmd);
    $systemCmd = "attrib -A ".$epocroot.$armLibDir."\\*.*";
    runSystemCmd($systemCmd);

    # Build Qt
    runSystemCmd("qmake");
    runSystemCmd("bldmake bldfiles");
    runSystemCmd("abld build armv5 udeb");

    # Make a temporary dir structure
    system("rd /S /Q $tempDir 2> NUL");
    runSystemCmd("mkdir $tempDir\\$armDbgDir");
    runSystemCmd("mkdir $tempDir\\$armRelDir");
    runSystemCmd("mkdir $tempDir\\$gcceDbgDir");
    runSystemCmd("mkdir $tempDir\\$gcceRelDir");
    runSystemCmd("mkdir $tempDir\\$armLibDir");
    $systemCmd = "xcopy ".$epocroot.$armDbgDir."\\*.* ".$tempDir."\\".$armDbgDir."\\*.* /A /Q";
    runSystemCmd($systemCmd);
    $systemCmd = "xcopy ".$tempDir."\\".$armDbgDir."\\*.* ".$tempDir."\\".$armRelDir."\\*.* /Q";
    runSystemCmd($systemCmd);
    $systemCmd = "del /F /Q ".$tempDir."\\".$armRelDir."\\*.sym";
    runSystemCmd($systemCmd);
    $systemCmd = "xcopy ".$tempDir."\\".$armDbgDir."\\*.* ".$tempDir."\\".$gcceDbgDir."\\*.* /Q";
    runSystemCmd($systemCmd);
    $systemCmd = "xcopy ".$tempDir."\\".$armRelDir."\\*.* ".$tempDir."\\".$gcceRelDir."\\*.* /Q";
    runSystemCmd($systemCmd);
    $systemCmd = "xcopy ".$epocroot.$armLibDir."\\*.* ".$tempDir."\\".$armLibDir."\\*.* /A /Q";
    runSystemCmd($systemCmd);

    # Create sis to send for signing
    my @sisFileNameList = split(/\./, $pkgFileName);
    pop(@sisFileNameList);
    my $sisFileName = join("", @sisFileNameList).".sis";
    system("del /F /Q $sisFileName 2> NUL");
    runSystemCmd("makesis $pkgFileName $sisFileName");
    $systemCmd = "xcopy $sisFileName ".$tempDir."\\ /Q";
    runSystemCmd($systemCmd);
    system("del /F /Q $sisFileName 2> NUL");
}
else
{
    print("Usage:\n");
    print("build_bin_package.pl qt_deployment_armv5_udeb.pkg [EPOCROOT]\n");
    print("EPOCROOT is optional, defaults to '\\'\n");
    print("1) Clean up the env. (abld reallyclean & delete \\my\\epoc\\root\\build folder)\n");
    print("2) Run \"build_bin_package.pl qt_deployment_armv5_udeb.pkg [\\my\\epoc\\root\\]>\"\n");
    print("   to prepare, build, and create the package structure and sis file.\n");
    print("   Note: Run in the directory you want to be built for package contents.\n");
    print("3) Get the sis signed and replace it with signed sisx.\n");
    print("4) Zip up the $tempDir contents\n");
}

sub runSystemCmd
{
    my $error_code = system($_[0]);
    if ($error_code != 0)
    {
        print("'$_[0]' call failed: error code == $error_code\n");
        exit;
    }
}
