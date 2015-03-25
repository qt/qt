/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the utils of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/
#include <QtCore/QtCore>

// Scans files for characters >127 and replaces them with the \nnn octal representation

int main(int argc, char *argv[])
{
    if (argc <= 1)
        qFatal("Usage: %s FILES", argc ? argv[0] : "fixnonlatin1");
    for (int i = 1; i < argc; ++i) {

        QString fileName = QString::fromLocal8Bit(argv[i]);
        if (   fileName.endsWith(".gif")
            || fileName.endsWith(".jpg")
            || fileName.endsWith(".tif")
            || fileName.endsWith(".tiff")
            || fileName.endsWith(".png")
            || fileName.endsWith(".mng")
            || fileName.endsWith(".ico")
            || fileName.endsWith(".zip")
            || fileName.endsWith(".gz")
            || fileName.endsWith(".qpf")
            || fileName.endsWith(".ttf")
            || fileName.endsWith(".pfb")
            || fileName.endsWith(".exe")
            || fileName.endsWith(".nib")
            || fileName.endsWith(".o")
            )
            continue;

        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            qFatal("Cannot open '%s': %s", argv[i], qPrintable(file.errorString()));

        QByteArray ba = file.readAll();
        bool mod = false;
        for (int j = 0; j < ba.count(); ++j) {
            uchar c = ba.at(j);
            if (c > 127) {
                ba[j] = '\\';
                ba.insert(j + 1, QByteArray::number(c, 8).rightJustified(3, '0', true));
                j += 3;
                mod = true;
            }
        }
        file.close();

        if (!mod)
            continue;

        qWarning("found non-latin1 characters in '%s'", argv[i]);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qWarning("Cannot open '%s' for writing: %s", argv[i], qPrintable(file.errorString()));
            continue;
        }
        if (file.write(ba) < 0)
            qFatal("Error while writing into '%s': %s", argv[i], qPrintable(file.errorString()));
        file.close();
    }

    return 0;
}

