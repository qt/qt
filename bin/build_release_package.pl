#######################################################################
#
# A script for creating binary release package
#
# Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
# Contact: Qt Software Information (qt-info@nokia.com)
#
#######################################################################

use Cwd;

if (@ARGV)
{
    my $platform = shift(@ARGV);
    my $build = shift(@ARGV);
    my $cert = shift(@ARGV);
    my $certKey = shift(@ARGV);
    my $releaseDir = shift(@ARGV);
    my $qtRootDir = shift(@ARGV);
    my $epocroot = shift(@ARGV);

    if ($build eq "")
    {
        print("HW build parameter required!\n");
        exit 1;
    }

    if ($cert eq "")
    {
        print("Signing certificate parameter required!\n");
        exit 2;
    }

    if ($certKey eq "")
    {
        print("Signing certificate key parameter required!\n");
        exit 3;
    }

    if ($releaseDir eq "")
    {
        print("Release directory parameter required!\n");
        exit 4;
    }

    if ($qtRootDir eq "")
    {
        $qtRootDir = cwd();
        $qtRootDir =~ s/\//\\/g;
    }

    # Lose the ending separator is any
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

    my $hwBuildDir = "${epocroot}\\epoc32\\release\\$platform\\$build";
    my $armDbgDir = "epoc32\\release\\armv5\\udeb";
    my $armRelDir = "epoc32\\release\\armv5\\urel";
    my $gcceDbgDir = "epoc32\\release\\gcce\\udeb";
    my $gcceRelDir = "epoc32\\release\\gcce\\urel";
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
    runSystemCmd("configure -platform win32-mwc -xplatform symbian-abld -openssl-linked -qt-sql-sqlite -system-sqlite -nokia-developer");
    chdir("src");
    runSystemCmd("qmake");
    runSystemCmd("bldmake bldfiles");
    runSystemCmd("abld build ${platform} ${build}");
    runSystemCmd("abld build winscw udeb");
    chdir($qtRootDir);

    # Copy misc stuff
    runSystemCmd("xcopy ${qtRootDir}\\bin\\* ${releaseDirQt}\\bin /F /R /Y /I /D");
    runSystemCmd("xcopy ${qtRootDir}\\lib\\*.prl ${releaseDirQt}\\lib /F /R /Y /I /D /E");
    runSystemCmd("xcopy ${qtRootDir}\\mkspecs\\* ${releaseDirQt}\\mkspecs /F /R /Y /I /D /E");
    print("Copying includes...\n");
    runSystemCmd("xcopy ${qtRootDir}\\include\\* ${releaseDirQt}\\include /F /R /Y /I /D /E /Q");
    runSystemCmd("copy src\\corelib\\global\\qconfig.h ${releaseDirQt}\\src\\corelib\\global\\qconfig.h");
    runSystemCmd("copy .qmake.cache ${releaseDirQt}\\.qmake.cache");

    # Copy stuff indicated by pkg file for emulator
    open (PKG, "<".$pkgFileName);
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
                $pkgDestinationPath =~ s/!:/${releaseDirEpocroot}\\epoc32\\winscw\\c/g;
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

    # Copy any other binaries and related files built not included in pkg
    runSystemCmd("xcopy ${hwBuildDir}\\* ${releaseDirEpocroot}\\${armDbgDir} /A /F /R /Y /I /D ");
    runSystemCmd("xcopy ${releaseDirEpocroot}\\${armDbgDir}\\* ${releaseDirEpocroot}\\${armRelDir} /F /R /Y /I /D");
    system("del /F /Q ${releaseDirEpocroot}\\${armRelDir}\\*.sym 2> NUL");
    runSystemCmd("xcopy ${releaseDirEpocroot}\\${armDbgDir}\\* ${releaseDirEpocroot}\\${gcceDbgDir} /F /R /Y /I /D");
    runSystemCmd("xcopy ${releaseDirEpocroot}\\${armRelDir}\\* ${releaseDirEpocroot}\\${gcceRelDir} /F /R /Y /I /D");
    runSystemCmd("xcopy ${epocroot}\\${armLibDir}\\* ${releaseDirEpocroot}\\${armLibDir} /A /F /R /Y /I /D");
    runSystemCmd("xcopy ${epocroot}\\${winscwDbgDir}\\* ${releaseDirEpocroot}\\${winscwDbgDir} /A /F /R /Y /I /D");

    # Create unsigned sis and Rnd signed sisx
    my @pkgPathElements = split(/\\/, $pkgFileName);
    my $pathlessPkgFile = pop(@pkgPathElements);
    my @pkgSuffixElements = split(/\./, $pathlessPkgFile);
    pop(@pkgSuffixElements);
    my $sisFileName = join("", @pkgSuffixElements).".sis";
    my $rndSisFileName = join("", @pkgSuffixElements)."_rnd.sisx";
    system("mkdir ${releaseDirSis}");
    runSystemCmd("makesis ${pkgFileName} ${releaseDirSis}\\${sisFileName}");
    runSystemCmd("signsis ${releaseDirSis}\\${sisFileName} ${releaseDirSis}\\${rndSisFileName} ${cert} ${certKey}");
}
else
{
    print("Usage:\n");
    print("build_release_package.pl <hw platform> <hw build> <signing cert> <signing cert key> <release directory> [QTROOT] [EPOCROOT]\n");
    print("QTROOT and EPOCROOT are optional; QTROOT defaults to current dir and EPOCROOT defaults to '\\'\n");
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
        exit 5;
    }
}
