#!\usr\bin\perl
############################################################################################
#
# Convenience script for creating signed packages you can install on your phone.
#
# Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
# Contact: Qt Software Information (qt-info@nokia.com)
#
############################################################################################

use strict;

# use a command-line parsing module
use Getopt::Long;
# Use file name parsing module
use File::Basename;

sub Usage() {
    print "\n";
    print "==========================================================================================\n";
    print "Convenience script for creating signed packages you can install on your phone.\n";
    print "\n";
    print "Usage: createpackage.pl [-i] templatepkg platform target [certificate key [passphrase]]\n";
    print "\n";
    print "Where parameters are as follows:\n";
    print "     [-i|install] = Install the package right away using PC suite\n";
    print "     templatepkg  = Name of .pkg file template\n";
    print "     platform     = One of the supported platform\n";
    print "                    GCCE | ARMV5 | ARMV6 | ARMV7\n";
    print "     target       = Either UDEB or UREL\n";
    print "     certificate  = The certificate file used for signing\n";
    print "     key          = The certificate's private key file\n";
    print "     passphrase   = The certificate's private key file's passphrase\n";
    print "\n";
    print "If no certificate and key files are provided, either a RnD certificate or\n";
    print "a self-signed certificate from Qt installation root directory is used.\n";
    print "\n";
    print "==========================================================================================\n";
    exit();
}

# Read given options
my $install = "";
unless (GetOptions('i|install' => \$install)){
    Usage();
}

# Read params to variables
my $templatepkg = $ARGV[0];
my $platform = uc $ARGV[1];
my $target = uc $ARGV[2];
my $visualtarget = $target;
$visualtarget =~ s/udeb/debug/i;
$visualtarget =~ s/urel/release/i;

my $certificate = $ARGV[3];
my $key = $ARGV[4];
my $passphrase = $ARGV[5];

# Generate output pkg basename (i.e. file name without extension)
my $pkgoutputbasename = $templatepkg;
$pkgoutputbasename =~ s/_template\.pkg/_$visualtarget-$platform/g;
$pkgoutputbasename = lc($pkgoutputbasename);

# Store output file names to variables
my $pkgoutput = lc($pkgoutputbasename.".pkg");
my $unsigned_sis_name = $pkgoutputbasename."_unsigned.sis";
my $signed_sis_name = $pkgoutputbasename.".sis";

# Store some utility variables
my $scriptpath = dirname(__FILE__);
my $certtext = $certificate;
my $certpath = $scriptpath;
$certpath =~ s-^(.*[^\\])$-$1\\-o;      # ensure path ends with a backslash
$certpath = $certpath."../";            # certificates are one step up in hierarcy
$certpath =~ s-/-\\-go;	                # for those working with UNIX shells

# Check some pre-conditions and print error messages if needed
unless (length($templatepkg) && length($platform) && length($target)) {
    print "\nError: Template PKG filename, platform or target is not defined!\n";
    Usage();
}

# Check template exist
stat($templatepkg);
unless( -e _ ) {
    print "\nError: Package description file '$templatepkg' does not exist!\n";
    Usage();
}

# Check certifcate preconditions and set default certificate variables if needed
if (length($certificate)) {
    unless(length($key)) {
        print "\nError: Custom certificate key file parameter missing.!\n";
        Usage();
    }
} else {
    #If no certificate is given, check default options
    $certtext = "RnD";
    $certificate = $certpath."rd.cer";
    $key = $certpath."rd-key.pem";

    stat($certificate);
    unless( -e _ ) {
        $certtext = "Self Signed";
        $certificate = $certpath."selfsigned.cer";
        $key = $certpath."selfsigned.key";
    }
}

# Remove any existing .sis packages
unlink $unsigned_sis_name;
unlink $signed_sis_name;
unlink $pkgoutput;

# Preprocess PKG
local $/;
# read template file
open( TEMPLATE, $templatepkg) or die "Error '$templatepkg': $!\n";
$_=<TEMPLATE>;
close (TEMPLATE);

# replace the PKG variables
s/\$\(PLATFORM\)/$platform/gm;
s/\$\(TARGET\)/$target/gm;

#write the output
open( OUTPUT, ">$pkgoutput" ) or die "Error '$pkgoutput' $!\n";
print OUTPUT $_;
close OUTPUT;

# Create and sign SIS
system ("makesis $pkgoutput $unsigned_sis_name");
system ("signsis $unsigned_sis_name $signed_sis_name $certificate $key $passphrase");

# Check if creating signed SIS Succeeded
stat($signed_sis_name);
if( -e _ ) {
    print ("\nSuccessfully created $signed_sis_name using certificate $certtext!\n");

    # remove temporary pkg and unsigned sis
    unlink $pkgoutput;
    unlink $unsigned_sis_name;

    # Install the sis if requested
    if ($install) {
        print ("\nInstalling $signed_sis_name...\n");
        system ("$signed_sis_name");
    }
} else {
    # Lets leave the generated PKG for problem solving purposes
    print ("\nSIS creation failed!\n");
}


#end of file
