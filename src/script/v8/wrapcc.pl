#!/usr/bin/perl
scalar(@ARGV) == 2 or die "Usage: $0 INFILE OUTFILE";
my $inFile = $ARGV[0];
my $outFile = $ARGV[1];
open FILE, ">", $outFile or die "Error: could not open $outFile for writing";
print FILE "#include \"$inFile\"\n";
close FILE;
