#!/usr/bin/perl -w

use strict;

#globals
my %VARS = ( 
    "docs" => "/Developer/Documentation/Qt", 
    "libs" => "/Library/Frameworks", 
    "plug" => "/Developer/Applications/Qt/plugins", 
    "trns" => "/Developer/Applications/Qt/translations",
    "data" => "/usr/local/Qt" . $ENV{"VERSION_MAJOR"} . "." . $ENV{"VERSION_MINOR"},
    "bins" => "/Developer/Tools/Qt",
    "demo" => "/Developer/Examples/Qt/Demos",
    "xmpl" => "/Developer/Examples/Qt/",
    "hdrs" => "/usr/include",
    "prfx" => "/"
);
my $file_in = 0;
my $file_out = 0;

#parse args
while ( @ARGV ) {
    my $arg = shift @ARGV;
    if($arg =~ /^-/) {
	my $val = shift @ARGV;
	my $var = $arg;
	$var =~ s,^-,,;
	$VARS{$var} = $val;
    } elsif(!$file_in) {
	$file_in = $arg;
    } elsif(!$file_out) {
	$file_out = $arg;
    } else {
	die "$0 <file_in> <file_out>";
    }
}
die "$0 <file_in> <file_out>" if(!$file_in || !$file_out);

#open

open(FIN , "<" . $file_in ) || die "Could not open $file_in for reading";
open(FOUT, ">" . $file_out) || die "Could not open $file_out for writing";
local $/;
binmode FIN;
binmode FOUT;

#do stuff
while(my $CONTENT = <FIN>) {
    foreach (keys(%VARS)) {
	my $var = "qt_" . $_ . "path=";
	my $val = $VARS{$_};
	my $val_length = length($val)+1;
	$CONTENT =~ s,${var}.{$val_length},${var}${val}\0,g;
    }
    print FOUT $CONTENT;
}

#close
close FIN;
close FOUT;
