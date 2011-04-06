#!/usr/bin/perl
#############################################################################
##
## Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
## All rights reserved.
## Contact: Nokia Corporation (qt-info@nokia.com)
##
## This file is part of the Declarative module of the Qt Toolkit.
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
