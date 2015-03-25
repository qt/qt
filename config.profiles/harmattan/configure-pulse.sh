#/bin/sh
#############################################################################
##
## Copyright (C) 2015 The Qt Company Ltd.
## Contact: http://www.qt.io/licensing/
##
## This file is the build configuration utility of the Qt Toolkit.
##
## $QT_BEGIN_LICENSE:LGPL$
## Commercial License Usage
## Licensees holding valid commercial Qt licenses may use this file in
## accordance with the commercial license agreement provided with the
## Software or, alternatively, in accordance with the terms contained in
## a written agreement between you and The Qt Company. For licensing terms
## and conditions see http://www.qt.io/terms-conditions. For further
## information use the contact form at http://www.qt.io/contact-us.
##
## GNU Lesser General Public License Usage
## Alternatively, this file may be used under the terms of the GNU Lesser
## General Public License version 2.1 or version 3 as published by the Free
## Software Foundation and appearing in the file LICENSE.LGPLv21 and
## LICENSE.LGPLv3 included in the packaging of this file. Please review the
## following information to ensure the GNU Lesser General Public License
## requirements will be met: https://www.gnu.org/licenses/lgpl.html and
## http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
##
## As a special exception, The Qt Company gives you certain additional
## rights. These rights are described in The Qt Company LGPL Exception
## version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
##
## GNU General Public License Usage
## Alternatively, this file may be used under the terms of the GNU
## General Public License version 3.0 as published by the Free Software
## Foundation and appearing in the file LICENSE.GPL included in the
## packaging of this file.  Please review the following information to
## ensure the GNU General Public License version 3.0 requirements will be
## met: http://www.gnu.org/copyleft/gpl.html.
##
## $QT_END_LICENSE$
##
#############################################################################


# Fail on error
set -e
# We will assume that if TOOLCHAIN_PATH is not set, that other required actions have not been done either
# so advise the user - N.B.: PATH & PKG_CONFIG_PATH may already be set to system values so we can't
# simply check for them being unset.
if [ -z "$TOOLCHAIN_PATH" ]; then
    echo
    echo "TOOLCHAIN_PATH    must be set to the path of the columbus toolchain, e.g.:" 1>&2
    echo "                      export TOOLCHAIN_PATH=/opt/toolchains/columbus/cs2007q3-glibc2.5-arm7" 1>&2
    echo "PATH              should have the path to toolchain's bin dir at beginning, e.g.:" 1>&2
    echo "                      export PATH=\$TOOLCHAIN_PATH/bin:\$PATH" 1>&2
    echo "PKG_CONFIG_PREFIX should have the prefix for pkg config, e.g.:" 1>&2
    echo "                      export PKG_CONFIG_PREFIX=\$TOOLCHAIN_PATH/arm-none-linux-gnueabi" 1>&2
    echo "PKG_CONFIG_PATH   must be set to the path(s) to pkg config .pc file location(s), e.g.:" 1>&2
    echo "                      export PKG_CONFIG_PATH=\$PKG_CONFIG_PREFIX/libc/lib/pkgconfig:\$PKG_CONFIG_PREFIX/libc/usr/lib/pkgconfig" 1>&2
    echo
    exit 1
fi

# We assume the current dir is the depot and we are not shadow building
# Blast the mkspec we use, if it exists, and copy it out of debian dir
rm -rf mkspecs/linux-g++-cross
cp -a debian/mkspecs/linux-g++-cross mkspecs/

# maemo does the next two lines, no idea why, left them for referance
# rm -rf mkspecs/glibc-g++
# cp -a mkspecs/linux-g++ mkspecs/glibc-g++

# Run configure - we take extra arguments if given
exec ./configure -nokia-developer \
    -prefix "/usr" \
    -bindir "/usr/bin" \
    -libdir "/usr/lib" \
    -docdir "/usr/share/qt4/doc" \
    -headerdir "/usr/include/qt4" \
    -datadir "/usr/share/qt4" \
    -plugindir "/usr/lib/qt4/plugins" \
    -translationdir "/usr/share/qt4/translations" \
    -sysconfdir "/etc/xdg" \
    -arch arm \
    -xplatform linux-g++-cross \
    -fast \
    -mitshm \
    -no-optimized-qmake \
    -reduce-relocations \
    -no-separate-debug-info \
    -system-zlib \
    -system-libtiff \
    -system-libpng \
    -system-libjpeg \
    -no-nas-sound \
    -qt-gif \
    -no-qt3support \
    -no-libmng \
    -opengl es2 \
    -no-accessibility \
    -nomake examples \
    -nomake demos \
    -little-endian \
    -I${TOOLCHAIN_PATH}/libc/usr/include/freetype2 \
    -lfontconfig \
    -no-cups \
    -no-gtkstyle \
    -exceptions \
    -no-xinerama \
    -dbus \
    -glib \
    -no-pch \
    -gstreamer \
    -svg \
    -webkit \
    -no-sql-ibase \
    -xmlpatterns \
    -system-sqlite \
    -plugin-sql-sqlite \
    -openssl \
    -DQT_QLOCALE_USES_FCVT \
    "$@"
# End of Script
