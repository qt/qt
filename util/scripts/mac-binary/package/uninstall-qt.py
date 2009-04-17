#!/usr/bin/env python

#############################################################################
##
## Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
## Contact: Qt Software Information (qt-info@nokia.com)
##
## This file is part of the $MODULE$ of the Qt Toolkit.
##
## $QT_BEGIN_LICENSE:LGPL$
## No Commercial Usage
## This file contains pre-release code and may not be distributed.
## You may use this file in accordance with the terms and conditions
## contained in the either Technology Preview License Agreement or the
## Beta Release License Agreement.
##
## GNU Lesser General Public License Usage
## Alternatively, this file may be used under the terms of the GNU Lesser
## General Public License version 2.1 as published by the Free Software
## Foundation and appearing in the file LICENSE.LGPL included in the
## packaging of this file.  Please review the following information to
## ensure the GNU Lesser General Public License version 2.1 requirements
## will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
##
## In addition, as a special exception, Nokia gives you certain
## additional rights. These rights are described in the Nokia Qt LGPL
## Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
## package.
##
## GNU General Public License Usage
## Alternatively, this file may be used under the terms of the GNU
## General Public License version 3.0 as published by the Free Software
## Foundation and appearing in the file LICENSE.GPL included in the
## packaging of this file.  Please review the following information to
## ensure the GNU General Public License version 3.0 requirements will be
## met: http://www.gnu.org/copyleft/gpl.html.
##
## If you are unsure which license is appropriate for your use, please
## contact the sales department at qt-sales@nokia.com.
## $QT_END_LICENSE$
##
## This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
## WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
##
#############################################################################

import commands, sys, os, shutil, glob

trace = False
justTest = False
# Globals
AllPackages = ['docs', 'headers', 'examples', 'plugins', 'tools', 'libraries', 'debuglibraries', 'translations' ]

packagesToRemove = AllPackages

# Travese the list and remove values that won't work.
def realPackagesToRemove(packageList):
    for package in packageList:
        if package not in AllPackages:
            print "%s is NOT a valid package, removing it from the list" % (package)
            packageList.remove(package)
    return packageList

#Remove the files in the list
def removeFiles(fileList, helperFunc=0):
    directories = []
    for file in fileList:
        file = file[1:]
        if not os.path.islink(file) and os.path.isdir(file):
            directories.append(file)
        elif len(file) > 0:
            if os.path.exists(file) or os.path.islink(file):
                if trace:
                    print "remove file: " + file
                if not justTest:
                    os.remove(file) 
            else:
                print "file: %s does not exist, skipping" % file


    # Now remove any empty directories
    directories.reverse()
    for dir in directories:
        if helperFunc:
            helperFunc(dir)
        if (os.path.exists(dir)) and len(os.listdir(dir)) == 0:
            if trace:
                print "remove dir: " + dir
            if not justTest:
                os.rmdir(dir)
        elif trace and os.path.exists(dir):
            print "NOT removing " + dir

def removeExampleGeneratedFiles(dir):
    extraList = []
    extraList.append(os.path.join(dir, "Info.plist"))
    extraList.append(os.path.join(dir, ".DS_Store"))
    extraList.append(os.path.join(dir, ".qmake.cache"))
    extraList.append(os.path.join(dir, "Makefile"))
    extraList.append(os.path.join(dir, ".obj"))
    extraList.append(os.path.join(dir, ".moc"))

    xcodeprojects = glob.glob(dir + "/*" + os.path.basename(dir) + "*.xcode*")
    xcodeprojects += glob.glob(dir + "/.xcode*")  # Grab the items from qmake mess-up in 4.0.1

    for file in xcodeprojects:
        if trace:
            print "  removing the tree in " + file
        shutil.rmtree(file)

    for file in extraList:
        if os.path.exists(file):
            if trace:
                print "  removing the file in " + file
            if os.path.isdir(file):
                shutil.rmtree(file)
            else:
                os.remove(file)
    

# Remove the package
def removePackage(package):
    print "removing package " + package
    realPackageName = "/Library/Receipts/Qt_" + package + ".pkg"
    bomLocation = os.path.join(realPackageName, "Contents/Archive.bom")
    if os.path.exists(realPackageName) and os.path.isdir(realPackageName):
        fileList = commands.getoutput("/usr/bin/lsbom -f -p f -d -l " + bomLocation).split()
        if len(fileList) > 0:
            if (package == "examples"):
                removeFiles(fileList, removeExampleGeneratedFiles)
            else:
                removeFiles(fileList)
            shutil.rmtree(realPackageName)
    else:
        print "%s is not installed, skipping." % package


################# Here's where the actual script starts ########################################
if os.getuid() != 0:
    print sys.argv[0] + ": This script must be run as root or with sudo, exiting now."
    sys.exit(-1)

# Take the names of packages on the command-line
if len(sys.argv) > 1:
    packagesToRemove = sys.argv[1:]

packagesToRemove = realPackagesToRemove(packagesToRemove)

if len(packagesToRemove) < 1:
    print "\nNo valid packages to uninstall.\nusage: %s [package ...]" % (sys.argv[0])
    sys.exit(1)

for package in packagesToRemove:
    removePackage(package)
