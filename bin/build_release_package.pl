#######################################################################
#
# A script for creating binary release package
#
# Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
# Contact: Qt Software Information (qt-info@nokia.com)
#
#######################################################################

my $buildContentDemoValue = "libs+demo";

if (@ARGV)
{
    my $buildContent = shift(@ARGV);
    my $platform = shift(@ARGV);
    my $build = shift(@ARGV);
    my $cert = shift(@ARGV);
    my $certKey = shift(@ARGV);
    my $releaseDir = shift(@ARGV);
    my $qtRootDir = shift(@ARGV);
    my $epocroot = shift(@ARGV);

    if ($buildContent eq "")
    {
        print("Build content parameter required!\n");
        exit 2;
    }

    if ($platform eq "")
    {
        print("HW platform parameter required!\n");
        exit 2;
    }

    if ($build eq "")
    {
        print("HW build parameter required!\n");
        exit 2;
    }

    if ($cert eq "")
    {
        print("Signing certificate parameter required!\n");
        exit 2;
    }

    if ($certKey eq "")
    {
        print("Signing certificate key parameter required!\n");
        exit 2;
    }

    if ($releaseDir eq "")
    {
        print("Release directory parameter required!\n");
        exit 2;
    }

    if ($qtRootDir eq "")
    {
        print("Qt root directory parameter required!\n");
        exit 2;
    }

    if ($epocroot eq "")
    {
        print("Epocroot directory parameter required!\n");
        exit 2;
    }

    # Lose the ending separator if any
    $qtRootDir =~ s/\\$//;
    $epocroot =~ s/\\$//;

    chdir($qtRootDir);

    # Lose the drive designator from epocroot
    $epocroot =~ s/.://;

    my $releaseDirQt = "${releaseDir}\\qt";
    my $releaseDirEpocroot = "${releaseDir}\\epocroot";
    my $releaseDirSis = $releaseDirQt; # Put sis files to Qt source dir root

    my $qtRootDirForMatch = $qtRootDir; # Double backslashes so that variable can be used in matching
    $qtRootDirForMatch =~ s/\\/\\\\/g;

    my $pkgFileName = "src\\s60installs\\qt_libs_${platform}_${build}.pkg";
    my $demoAppPkgFileName = "demos\\embedded\\fluidlauncher\\fluidlauncher_${platform}_${build}.pkg";
    my $sisFileNameBase = "qt_libs";
    my $demoAppSisFileNameBase = "fluidlauncher";

    my $hwBuildDir = "${epocroot}\\epoc32\\release\\$platform\\$build";
    my $armDbgDir = "epoc32\\release\\armv5\\udeb";
    my $armRelDir = "epoc32\\release\\armv5\\urel";
    my $armLibDir = "epoc32\\release\\armv5\\lib";
    my $winscwDbgDir = "epoc32\\release\\winscw\\udeb";

    # clean up old stuff
    system("rd /S /Q ${releaseDir} 2> NUL");
    system("rd /S /Q ${epocroot}\\epoc32\\build 2> NUL"); # Just in case env is not clean

    # Copy the whole thing over to the release dir before it gets polluted
    print("Copying clean tree...\n");
    my $tempExcludeFile = "${releaseDir}\\__temp_exclude.txt";
    system("mkdir ${releaseDir} 2>NUL");
    runSystemCmd("echo ${qtRootDir}\\.git\\ > ${tempExcludeFile}");
    runSystemCmd("echo ${qtRootDir}\\tests\\ >> ${tempExcludeFile}");
    runSystemCmd("echo ${qtRootDir}\\util\\ >> ${tempExcludeFile}");
    runSystemCmd("xcopy ${qtRootDir} ${releaseDirQt} /E /I /H /Q /EXCLUDE:${tempExcludeFile}");
    system("del /F /Q ${tempExcludeFile} 2> NUL");

    # Clear archive flag from all items in \epoc32\release\armv5\urel and \epoc32\release\armv5\lib
    # as those will have the binaries used for all platforms and builds.
    runSystemCmd("attrib -A ${hwBuildDir}\\*");
    runSystemCmd("attrib -A ${epocroot}\\${armLibDir}\\*");
    runSystemCmd("attrib -A ${epocroot}\\${winscwDbgDir}\\*");

    # Build Qt
    runSystemCmd("configure -platform win32-mwc -xplatform symbian-abld -openssl-linked -qt-sql-sqlite -system-sqlite -confirm-license -opensource");
    buildDir("src", $qtRootDir, $platform, $build);

    # Copy misc stuff
    runSystemCmd("xcopy ${qtRootDir}\\bin\\* ${releaseDirQt}\\bin /F /R /Y /I /D");
    runSystemCmd("xcopy ${qtRootDir}\\lib\\*.prl ${releaseDirQt}\\lib /F /R /Y /I /D /E");
    runSystemCmd("xcopy ${qtRootDir}\\mkspecs\\* ${releaseDirQt}\\mkspecs /F /R /Y /I /D /E");
    print("Copying includes...\n");
    runSystemCmd("xcopy ${qtRootDir}\\include\\* ${releaseDirQt}\\include /F /R /Y /I /D /E /Q");
    runSystemCmd("copy src\\corelib\\global\\qconfig.h ${releaseDirQt}\\src\\corelib\\global\\qconfig.h");
    runSystemCmd("copy .qmake.cache ${releaseDirQt}\\.qmake.cache");

    # Copy stuff indicated by pkg file for emulator
    parsePkgFile($pkgFileName, $epocroot, $build, $platform, $releaseDirEpocroot, $qtRootDirForMatch, $releaseDirQt);

    # Copy any other binaries and related files built not included in pkg
    runSystemCmd("xcopy ${hwBuildDir}\\* ${releaseDirEpocroot}\\${armDbgDir} /A /F /R /Y /I /D ");
    runSystemCmd("xcopy ${releaseDirEpocroot}\\${armDbgDir}\\* ${releaseDirEpocroot}\\${armRelDir} /F /R /Y /I /D");
    system("del /F /Q ${releaseDirEpocroot}\\${armRelDir}\\*.sym 2> NUL");
    runSystemCmd("xcopy ${epocroot}\\${armLibDir}\\* ${releaseDirEpocroot}\\${armLibDir} /A /F /R /Y /I /D");
    runSystemCmd("xcopy ${epocroot}\\${winscwDbgDir}\\* ${releaseDirEpocroot}\\${winscwDbgDir} /A /F /R /Y /I /D");

    if ($buildContent eq $buildContentDemoValue)
    {
        # Also build demos & examples and add fluidlauncher sis
        buildDir("examples", $qtRootDir, $platform, $build);
        buildDir("demos", $qtRootDir, $platform, $build);
        parsePkgFile($demoAppPkgFileName, $epocroot, $build, $platform, $releaseDirEpocroot, $qtRootDirForMatch, $releaseDirQt);
        createSis($demoAppPkgFileName, $releaseDirSis, "selfsigned.cer", "selfsigned.key", $demoAppSisFileNameBase);
    }

    # Create unsigned sis and Rnd signed sisx
    createSis($pkgFileName, $releaseDirSis, $cert, $certKey, $sisFileNameBase);

}
else
{
    print("Usage:\n");
    print("build_release_package.pl <build content (${buildContentDemoValue}|libs)> <hw platform> <hw build> <signing cert> <signing cert key> <release directory> <QTROOT> <EPOCROOT>\n");
    print("1) Clean up the env. (abld reallyclean & delete \\my\\epoc\\root\\build folder)\n");
    print("2) Run \"build_release_package.pl armv5 udeb \\rd.cer \\rd-key.pem \\my_release_dir [\\my\\qt\\root\\] [\\my\\epoc\\root\\]>\"\n");
    print("   to build Qt and create the release package structure and sis file.\n");
    print("   Note: Run in the Qt root directory.\n");
    print("3) Optional: Get the sis signed with commercial certificate (can be found in qt directory under release directory)\n");
    print("4) Zip up the release directory contents\n");
}

sub runSystemCmd
{
    my $error_code = system($_[0]);
    if ($error_code != 0)
    {
        print("'$_[0]' call failed: error code == $error_code\n");
        exit 9;
    }
}

sub parsePkgFile
{
    my $epocroot = $_[1];
    my $build = $_[2];
    my $platform = $_[3];
    my $releaseDirEpocroot = $_[4];
    my $qtRootDirForMatch = $_[5];
    my $releaseDirQt = $_[6];

    open (PKG, "<".$_[0]);
    while (<PKG>)
    {
        my $line = $_;
        chomp ($line);

        # If the line specifies a file, parse the source and destination locations.
        if ($line =~ m/\"([^\"]+)\"\s*\-\s*\"([^\"]+)\"/)
        {
            my $sourcePath = $1;
            my $pkgDestinationPath = $2;
            $sourcePath =~ s/\//\\/g;

            # Copy stuff required for creating deployment package
            my $destinationPath = $sourcePath;

            if($destinationPath =~ m/($qtRootDirForMatch)/i)
            {
                $destinationPath =~ s/($qtRootDirForMatch)//i;
                $destinationPath = $releaseDirQt.$destinationPath;
            }
            else
            {
                $destinationPath =~ s/.://;
                if ($epocroot !~ m/\\/)
                {
                    $destinationPath =~ s/($epocroot)//i;
                }
                $destinationPath = $releaseDirEpocroot.$destinationPath;
            }

            runSystemCmd("echo f|xcopy ${sourcePath} ${destinationPath} /F /R /Y /I /D");

            if ($sourcePath !~ m/\\epoc32\\release\\([^\\]+)\\(udeb|urel)\\(\w+(\.dll|\.exe))/i)
            {
                # Copy non-binaries also over for emulator to use
                if ($pkgDestinationPath =~ m/(private\\10003a3f\\import\\apps|resource\\apps)/i)
                {
                    $pkgDestinationPath =~ s/!:/${releaseDirEpocroot}\\epoc32\\release\\winscw\\udeb\\z/g;
                }
                else
                {
                    $pkgDestinationPath =~ s/!:/${releaseDirEpocroot}\\epoc32\\winscw\\c/g;
                }

                runSystemCmd("echo f|xcopy ${sourcePath} ${pkgDestinationPath} /F /R /Y /I /D");
            }
            else
            {
                # For binaries, copy also winscw versions (needed for deployed binaries that are not actually built)
                $sourcePath =~ s/epoc32\\release\\${platform}\\${build}/epoc32\\release\\winscw\\udeb/;
                $destinationPath =~ s/epoc32\\release\\${platform}\\${build}/epoc32\\release\\winscw\\udeb/;
                runSystemCmd("echo f|xcopy ${sourcePath} ${destinationPath} /F /R /Y /I /D");
            }

        }
    }
    close (PKG);
}

sub createSis
{
    my $pkgFileName = $_[0];
    my $releaseDirSis = $_[1];
    my $cert = $_[2];
    my $certKey = $_[3];
    my $sisFileNameBase = $_[4];
    my $signedSuffix = ".sis";
    my $unsignedSuffix = "_unsigned.sis";

    if ($cert =~ m/rd\.cer/i)
    {
        $signedSuffix = "_rnd.sis";
    }

    my $sisFileName = $sisFileNameBase.$unsignedSuffix;
    my $signedSisFileName = $sisFileNameBase.$signedSuffix;
    system("mkdir ${releaseDirSis} 2> NUL");
    runSystemCmd("makesis ${pkgFileName} ${releaseDirSis}\\${sisFileName}");
    runSystemCmd("signsis ${releaseDirSis}\\${sisFileName} ${releaseDirSis}\\${signedSisFileName} ${cert} ${certKey}");
}

sub buildDir
{
    my $buildDir = $_[0];
    my $qtRootDir = $_[1];
    my $platform = $_[2];
    my $build = $_[3];

    chdir($buildDir);
    runSystemCmd("qmake");
    runSystemCmd("bldmake bldfiles");
    runSystemCmd("abld build ${platform} ${build}");
    runSystemCmd("abld build winscw udeb");
    chdir($qtRootDir);
}
