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

sub Usage() {
    print("This script can be used to set capabilities of all binaries\n");
    print("specified for deployment in a .pkg file.\n");
    print("If no capabilities are given, the binaries will be given the\n");
    print("capabilities supported by self-signed certificates.\n");
    print("\n *** NOTE: If *_template.pkg file is given, 'target-platform' is REQUIRED. ***\n");
    print("\nUsage: patch_capabilities.pl pkg_filename <target-platform> [capability list]\n");
    print("\nE.g. patch_capabilities.pl myapp_template.pkg release-armv5 \"All -TCB\"\n");
    exit();
}

my @capabilitiesToSet = ("LocalServices", "NetworkServices", "ReadUserData", "UserEnvironment", "WriteUserData");

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
        unless ($targetplatform = shift(@ARGV))
        {
            Usage();
        }

        my @tmpvalues = split('-', $targetplatform);
        $target = $tmpvalues[0];
        $platform = $tmpvalues[1];

        # Convert visual target to real target (debug->udeb and release->urel)
        $target =~ s/debug/udeb/i;
        $target =~ s/release/urel/i;
    }

    # If the specified ".pkg" file exists (and can be read),
    if (($pkgFileName =~ m|\.pkg$|i) && -r($pkgFileName))
    {
        # If there are more arguments given, parse them as capabilities.
        if (@ARGV)
        {
            @capabilitiesToSet = ();
            while (@ARGV)
            {
                push (@capabilitiesToSet, pop(@ARGV));
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
            # Patch pkg UID
            my $line = $_;
            my $newLine = $line;
            if ($line =~ m/^\#.*\(0x[0-9|a-f|A-F]*\).*$/)
            {
                $newLine =~ s/\(0x./\(0xE/;
            }

            # Patch embedded sis name and UID
            if ($line =~ m/^@.*\.sis.*\(0x[0-9|a-f|A-F]*\).*$/)
            {
                $newLine =~ s/\(0x./\(0xE/;
                if ($line !~ m/^.*_selfsigned.sis.*$/)
                {
                    $newLine =~ s/\.sis/_selfsigned\.sis/i;
                }
            }

            # Remove all dependencies to other packages to reduce unnecessary error messages
            # from depended packages that are also patched and therefore have different UID.
            if ($line =~ m/^\(0x[0-9|a-f|A-F]*\).*\{.*\}$/)
            {
                $newLine = "\n"
            }

            # Remove manufacturer ifdef
            if ($line =~ m/^.*\(MANUFACTURER\)\=\(.*\).*$/)
            {
                $newLine = "\n";
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

            print NEW_PKG $newLine;

            chomp ($line);

            # If the line specifies a file, parse the source and destination locations.
            if ($line =~ m|\"([^\"]+)\"\s*\-\s*\"([^\"]+)\"|)
            {
                my $sourcePath = $1;
                my $destinationPath = $2;

                # If the given file is a binary, check the target and binary type (+ the actual filename) from its path.
                if ($sourcePath =~ m:/epoc32/release/([^/]+)/(udeb|urel|\$\(TARGET\))/(\w+(\.dll|\.exe)):i)
                {
                    # Do preprocessing for template pkg,
                    # In case of template pkg target and platform variables are set
                    if(length($target) && length($platform))
                    {
                        $sourcePath =~ s/\$\(PLATFORM\)/$platform/gm;
                        $sourcePath =~ s/\$\(TARGET\)/$target/gm;
                    }

                    push (@binaries, $sourcePath);
                }
            }
        }

        close (PKG);
        close (NEW_PKG);

        unlink($pkgFileName);
        rename($tempPkgFileName, $pkgFileName);

        print ("\n");

        my $baseCommandToExecute = "elftran -vid 0x0 -capability \"";
        if (@capabilitiesToSet)
        {
            $baseCommandToExecute .= join(" ", @capabilitiesToSet);
        }
        $baseCommandToExecute .= "\" ";

        # Actually set the capabilities of the listed binaries.
        foreach my $binaryPath(@binaries)
        {
            # Create the command line for setting the capabilities.
            my $commandToExecute = $baseCommandToExecute;
            $commandToExecute .= $binaryPath;

            # Actually execute the elftran command to set the capabilities.
            system ($commandToExecute." > NUL");
            print ("Executed ".$commandToExecute."\n");

            ## Create another command line to check that the set capabilities are correct.
            #$commandToExecute = "elftran -dump s ".$binaryPath;
        }

        print ("\n");
    }
}
else
{
    Usage();
}
