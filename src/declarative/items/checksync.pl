#!/usr/bin/perl
use strict;
use warnings;

die "Usage: $0 <QML directory>" if (@ARGV != 1);

my @excludes;
open (SYNCEXCLUDES, "<", "syncexcludes");
while (<SYNCEXCLUDES>) {
    if (/^([a-zA-Z0-9\._]+)/) {
        my $exclude = $1;
        push (@excludes, $exclude);
    }
}

my $portdir = ".";
my $qmldir = $ARGV[0];

opendir (PORTDIR, $portdir) or die "Cannot open port directory";
opendir (QMLDIR, $qmldir) or die "Cannot open QML directory";

my @portfiles = readdir(PORTDIR);
my @qmlfiles = readdir(QMLDIR);

closedir(PORTDIR);
closedir(QMLDIR);

foreach my $qmlfile (@qmlfiles) {
    if ($qmlfile =~ /^qdeclarative.*\.cpp$/ or $qmlfile =~ /qdeclarative.*\.h$/) {

        if (grep { $_ eq $qmlfile} @excludes) {
            next;
        }

        my $portfile = $qmlfile;
        $portfile =~ s/^qdeclarative/qsg/;

        if (grep { $_ eq $portfile} @portfiles) {

            open (PORTFILE, "<", "$portdir/$portfile") or die("Cannot open $portdir/$portfile for reading");

            my $firstline = <PORTFILE>;

            close (PORTFILE);

            if ($firstline and $firstline =~ /^\/\/ Commit: ([a-z0-9]+)/) {
                my $sha1 = $1;
                my $commitSha1 = "";

                my $output = `cd $qmldir; git log $qmlfile | head -n 1`;
                if ($output =~ /commit ([a-z0-9]+)/) {
                    $commitSha1 = $1;
                }

                if ($commitSha1 eq $sha1) {
                    print ("$portfile: OK\n");
                } else {
                    print ("$portfile: OUT OF DATE\n");
                }
            } else {
                print ("$portfile: OUT OF DATE\n");
            }
        } else {
            print ("$portfile: MISSING\n");
        }
    }
}
