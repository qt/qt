#!/usr/bin/perl

use warnings;
use strict;

my $count = shift;

open(MFIRST, "<english-m.txt") or die "Could not open male names";
open(FFIRST, "<english-f.txt") or die "Could not open female names";
open(SURNAME, "<english-s.txt") or die "Could not open surnames names";
open(ISP, "<email.txt") or die "Could not open isp names";

my @mnames = <MFIRST>;
my @fnames = <FFIRST>;
my @surnames = <SURNAME>;
my @isps = <ISP>;

print "BEGIN;\n";
print "CREATE TABLE contacts (recid INTEGER PRIMARY KEY, label TEXT, phone TEXT, email TEXT);\n";
print "CREATE INDEX contactSortOrder ON contacts(label, recid);\n";

my $i = 0;
while ($i < $count) {
    $i++;
    my $fn = randomFirstName();
    my $sn = randomLastName();
    my $em = randomEmail($fn, $sn);
    my $ph = randomPhoneNumber();

    $fn =~ s/'/''/g;
    $sn =~ s/'/''/g;
    $em =~ s/'/''/g;
    print "INSERT INTO contacts (label, email, phone) VALUES('$fn $sn', '$em', '$ph');\n"
}

print "COMMIT;\n";


sub randomFirstName
{
    my $name;
    if (int(rand 2) == 1) {
        $name = $mnames[int(rand @mnames)];
    } else {
        $name = $fnames[int(rand @fnames)];
    }
    chomp $name;
    $name;
}

sub randomLastName
{
    my $name = $surnames[int(rand @surnames)];
    chomp $name;
    $name;
}

sub randomEmail
{
    my ($fn, $ln) = @_;
    my $isp = $isps[int(rand @isps)];
    chomp $isp;
    my $path = int(rand 3);
    my $email;
    if ($path == 0) {
        $email = "$fn.$ln\@$isp";
    } elsif ($path == 1) {
        $email = "$fn\@$isp";
    } elsif ($path == 2) {
        $email = "$ln\@$isp";
    }
}

sub randomPhoneNumber
{
    my @digits = qw(1 2 3 4 5 6 7 8 9 0);
    my $phonenumber;
    for (1 .. 8) {
        $phonenumber .= $digits[int(rand @digits)];
    }
    $phonenumber;
}
