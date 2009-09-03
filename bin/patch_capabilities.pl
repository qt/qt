#######################################################################
#
# A script for setting binary capabilities based on .pkg file contents.
#
# Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
# Contact: Nokia Corporation (qt-info@nokia.com)
#
#######################################################################

sub Usage() {
    print("This script can be used to set capabilities of all binaries\n");
    print("specified for deployment in a .pkg file.\n");
    print("If no capabilities are given, the binaries will be given the\n");
    print("capabilities supported by self-signed certificates.\n");
    print("\nUsage: patch_capabilities.pl pkg_filename [target-platform] [capability list]\n");
    print("    If template .pkg file is given, next agrument must be 'target-platform'.\n");
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

        # Parse each line.
        while (<PKG>)
        {
            my $line = $_;
            my $newLine = $line;
            if ( $line =~ m/^\#.*\(0x[0-9|a-f|A-F]*\).*$/)
            {
                $newLine =~ s/\(0x./\(0xE/;
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
