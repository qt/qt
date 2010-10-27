#!/usr/bin/perl
#############################################################################
##
## Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
## All rights reserved.
## Contact: Nokia Corporation (qt-info@nokia.com)
##
## This file is part of the S60 port of the Qt Toolkit.
##
## $QT_BEGIN_LICENSE:LGPL$
## No Commercial Usage
## This file contains pre-release code and may not be distributed.
## You may use this file in accordance with the terms and conditions
## contained in the Technology Preview License Agreement accompanying
## this package.
##
## GNU Lesser General Public License Usage
## Alternatively, this file may be used under the terms of the GNU Lesser
## General Public License version 2.1 as published by the Free Software
## Foundation and appearing in the file LICENSE.LGPL included in the
## packaging of this file.  Please review the following information to
## ensure the GNU Lesser General Public License version 2.1 requirements
## will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
##
## In addition, as a special exception, Nokia gives you certain additional
## rights.  These rights are described in the Nokia Qt LGPL Exception
## version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
##
## If you have questions regarding the use of this file, please contact
## Nokia at qt-info@nokia.com.
##
##
##
##
##
##
##
##
## $QT_END_LICENSE$
##
#############################################################################

#######################################################################
#
# A script for setting binary capabilities based on .pkg file contents.
#
#######################################################################

#
# Note: Please make sure to output all changes done to the pkg file in a print statements
#       starting with "Patching: " to ease integration into IDEs!
#

use File::Copy;
use File::Spec;

sub Usage() {
    print("This script can be used to set capabilities of all binaries\n");
    print("specified for deployment in a .pkg file.\n");
    print("If no capabilities are given, the binaries will be given the\n");
    print("capabilities supported by self-signed certificates.\n\n");
    print(" *** NOTE: If *_template.pkg file is given and one is using symbian-abld or\n");
    print(" symbian-sbsv2 platform, 'target-platform' is REQUIRED. ***\n\n");
    print(" *** NOTE2: When patching gcce binaries built with symbian-sbsv2 toolchain,\n");
    print(" armv5 must be specified as platform.\n");
    print("\nUsage: patch_capabilities.pl pkg_filename [target-platform [capability list]]\n");
    print("\nE.g. patch_capabilities.pl myapp_template.pkg release-armv5 \"All -TCB\"\n");
    exit();
}

sub trim($) {
    my $string = shift;
    $string =~ s/^\s+//;
    $string =~ s/\s+$//;
    return $string;
}

my $nullDevice = "/dev/null";
$nullDevice = "NUL" if ($^O =~ /MSWin/);

my @capabilitiesToAllow = ("LocalServices", "NetworkServices", "ReadUserData", "UserEnvironment", "WriteUserData");
my @capabilitiesSpecified = ();

# If arguments were given to the script,
if (@ARGV)
{
    # Parse the first given script argument as a ".pkg" file name.
    my $pkgFileName = shift(@ARGV);

    # These variables will only be set for template .pkg files.
    my $target;
    my $platform;

    # Check if using template .pkg and set target/platform variables
    if (($pkgFileName =~ m|_template\.pkg$|i) && -r($pkgFileName))
    {
        my $targetplatform;
        my $templateFile;
        my $templateContents;
        open($templateFile, "< $pkgFileName") or die ("Could not open $pkgFileName");
        $templateContents = <$templateFile>;
        close($templateFile);
        unless (($targetplatform = shift(@ARGV)) || $templateContents !~ /\$\(PLATFORM\)/)
        {
            Usage();
        }
        $targetplatform = "-" if (!$targetplatform);
        my @tmpvalues = split('-', $targetplatform);
        $target = $tmpvalues[0];
        $platform = $tmpvalues[1];

        # Convert visual target to real target (debug->udeb and release->urel)
        $target =~ s/debug/udeb/i;
        $target =~ s/release/urel/i;

        if (($platform =~ m/^gcce$/i) && ($ENV{SBS_HOME})) {
            # Print a informative note in case suspected misuse is detected.
            print "\nNote: You must use armv5 as platform when packaging gcce binaries built using symbian-sbsv2 mkspec.\n";
        }
    }

    # If the specified ".pkg" file exists (and can be read),
    if (($pkgFileName =~ m|\.pkg$|i) && -r($pkgFileName))
    {
        print ("\n");
        print ("Patching package file and relevant binaries...\n");

        # If there are more arguments given, parse them as capabilities.
        if (@ARGV)
        {
            @capabilitiesSpecified = ();
            while (@ARGV)
            {
                push (@capabilitiesSpecified, pop(@ARGV));
            }
        }

        # Start with no binaries listed.
        my @binaries = ();

        my $tempPkgFileName = $pkgFileName."_@@TEMP@@";
        unlink($tempPkgFileName);
        open (NEW_PKG, ">>".$tempPkgFileName);
        open (PKG, "<".$pkgFileName);

        my $manufacturerElseBlock = 0;

        # Parse each line.
        while (<PKG>)
        {
            my $line = $_;
            my $newLine = $line;

            # Patch pkg UID if it's in protected range
            if ($line =~ m/^\#.*\((0x[0-7][0-9a-fA-F]*)\).*$/)
            {
                my $oldUID = $1;
                my $newUID = $oldUID;
                $newUID =~ s/0x./0xE/i;
                $newLine =~ s/$oldUID/$newUID/;
                print ("Patching: UID $oldUID is not compatible with self-signing! Changed to: $newUID.\n");
            }

            # Patch embedded sis name and UID if UID is in protected range
            if ($line =~ m/^@\"*(.*\.sis).*\((0x[0-7][0-9a-fA-F]*)\).*$/)
            {
                my $oldSisName = $1;
                my $oldUID = $2;
                my $newUID = $oldUID;
                $newUID =~ s/0x./0xE/i;
                $newLine =~ s/$oldUID/$newUID/;
                print ("Patching: Embedded sis $oldSisName UID $oldUID changed to: $newUID.\n");

                if ($oldSisName !~ m/^.*_selfsigned.sis$/i)
                {
                    my $newSisName = $oldSisName;
                    $newSisName =~ s/\.sis$/_selfsigned\.sis/i;
                    $newLine =~ s/$oldSisName/$newSisName/i;
                    print ("Patching: Embedded sis $oldSisName name changed to: $newSisName.\n");
                }
            }

            # Remove dependencies to known problem packages (i.e. packages that are likely to be patched, too)
            # to reduce unnecessary error messages.
            if ($line =~ m/^\((0x2002af5f)\).*\{.*\}$/)
            {
                $newLine = "\n";
                print ("Patching: Removed dependency to sqlite3.sis ($1) to avoid installation issues in case sqlite3.sis is also patched.\n");
            }
            if ($line =~ m/^\((0x2001E61C)\).*\{.*\}$/)
            {
                $newLine = "\n";
                print ("Patching: Removed dependency to qt.sis ($1) to avoid installation issues in case qt.sis is also patched.\n");
            }

            # Remove manufacturer ifdef
            if ($line =~ m/^.*\(MANUFACTURER\)\=\(.*\).*$/)
            {
                $newLine = "\n";
                print ("Patching: Removed manufacturer check as it is usually not desirable in self-signed packages.\n");
            }

            if ($line =~ m/^ELSEIF.*MANUFACTURER$/)
            {
                $manufacturerElseBlock = 1;
            }

            if ($manufacturerElseBlock eq 1)
            {
                $newLine = "\n";
            }

            if ($line =~ m/^ENDIF.*MANUFACTURER$/)
            {
                $manufacturerElseBlock = 0;
            }

            # If the line specifies a file, parse the source and destination locations.
            if ($line =~ m|^ *\"([^\"]+)\"\s*\-\s*\"([^\"]+)\"|)
            {
                my $sourcePath = $1;

                # If the given file is a binary, check the target and binary type (+ the actual filename) from its path.
                if ($sourcePath =~ m:\w+(\.dll|\.exe)$:i)
                {
                    # Do preprocessing for template pkg,
                    # In case of template pkg target and platform variables are set
                    if(length($target) && length($platform))
                    {
                        $sourcePath =~ s/\$\(PLATFORM\)/$platform/gm;
                        $sourcePath =~ s/\$\(TARGET\)/$target/gm;
                    }

                    # Change the source file name (but only if not already patched)
                    my $patchedSourcePath = $sourcePath;
                    if ($patchedSourcePath !~ m/_patched_caps/)
                    {
                        $newLine =~ s/(^.*)(\.dll|\.exe)(.*)(\.dll|\.exe)/$1_patched_caps$2$3$4/i;
                        $patchedSourcePath =~ s/(^.*)(\.dll|\.exe)/$1_patched_caps$2/i;

                        copy($sourcePath, $patchedSourcePath) or die "$sourcePath cannot be copied for patching.";
                    }
                    push (@binaries, $patchedSourcePath);
                }
            }

            print NEW_PKG $newLine;

            chomp ($line);
        }

        close (PKG);
        close (NEW_PKG);

        unlink($pkgFileName);
        rename($tempPkgFileName, $pkgFileName);

        print ("\n");

        my $baseCommandToExecute = "elftran -vid 0x0 -capability \"%s\" ";

        # Actually set the capabilities of the listed binaries.
        foreach my $binaryPath(@binaries)
        {
            # Create the command line for setting the capabilities.
            my ($binaryVolume, $binaryDirs, $binaryBaseName) = File::Spec->splitpath($binaryPath);
            my $commandToExecute = $baseCommandToExecute;
            my $executeNeeded = 0;
            if (@capabilitiesSpecified)
            {
                $commandToExecute = sprintf($baseCommandToExecute, join(" ", @capabilitiesSpecified));
                $executeNeeded = 1;
                my $capString = join(" ", @capabilitiesSpecified);
                print ("Patching: Patching the the Vendor ID to 0 and the capabilities used to: \"$capString\" in \"$binaryBaseName\".\n");
            } else {
                # Test which capabilities are present and then restrict them to the allowed set.
                # This avoid raising the capabilities of apps that already have none.
                my $dllCaps;
                open($dllCaps, "elftran -dump s $binaryPath |") or die ("Could not execute elftran");
                my $capsFound = 0;
                my $originalVid;
                my @capabilitiesToSet;
                my $capabilitiesToAllow = join(" ", @capabilitiesToAllow);
                my @capabilitiesToDrop;
                while (<$dllCaps>) {
                    if (/^Secure ID: ([0-7][0-9a-fA-F]*)$/) {
                        my $exeSid = $1;
                        if ($binaryBaseName =~ /\.exe$/) {
                            # Installer refuses to install protected executables in a self signed package, so abort if one is detected.
                            # We can't simply just patch the executable SID, as any registration resources executable uses will be linked to it via SID.
                            print ("Patching: Executable with SID in the protected range (0x$exeSid) detected: \"$binaryBaseName\". A self-signed sis with protected executables is not supported.\n");
                            exit(1);
                        }
                    }
                    if (/^Vendor ID: ([0-9a-fA-F]*)$/) {
                        $originalVid = "$1";
                    }
                    if (!$capsFound) {
                        $capsFound = 1 if (/Capabilities:/);
                    } else {
                        $_ = trim($_);
                        if ($capabilitiesToAllow =~ /$_/) {
                            push(@capabilitiesToSet, $_);
                        } else {
                            push(@capabilitiesToDrop, $_);
                        }
                    }
                }
                close($dllCaps);
                if ($originalVid !~ "00000000") {
                    print ("Patching: Vendor ID (0x$originalVid) incompatible with self-signed packages, setting it to zero for \"$binaryBaseName\".\n");
                    $executeNeeded = 1;
                }
                if ($#capabilitiesToDrop) {
                    my $capsToDropStr = join("\", \"", @capabilitiesToDrop);
                    $capsToDropStr =~ s/\", \"$//;

                    if ($binaryBaseName =~ /\.exe$/) {
                        # While libraries often have capabilities they do not themselves need just to enable them to be loaded by wider variety of processes,
                        # executables are more likely to need every capability they have been assigned or they won't function correctly.
                        print ("Patching: Executable with capabilities incompatible with self-signing detected: \"$binaryBaseName\". (Incompatible capabilities: \"$capsToDropStr\".) Reducing capabilities is only supported for libraries.\n");
                        exit(1);
                    } else {
                        print ("Patching: The following capabilities used in \"$binaryBaseName\" are not compatible with a self-signed package and will be removed: \"$capsToDropStr\".\n");
                        $executeNeeded = 1;
                    }
                }
                $commandToExecute = sprintf($baseCommandToExecute, join(" ", @capabilitiesToSet));
            }
            $commandToExecute .= $binaryPath;

            if ($executeNeeded) {
                # Actually execute the elftran command to set the capabilities.
                print ("\n");
                system ("$commandToExecute > $nullDevice");
            }
            ## Create another command line to check that the set capabilities are correct.
            #$commandToExecute = "elftran -dump s ".$binaryPath;
        }

        print ("\n");
        print ("NOTE: A patched package may not work as expected due to reduced capabilities and other modifications,\n");
        print ("      so it should not be used for any kind of Symbian signing or distribution!\n");
        print ("      Use a proper certificate to avoid the need to patch the package.\n");
        print ("\n");
    }
}
else
{
    Usage();
}
