/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the tools applications of the Qt Toolkit.
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

#include <QtGui>
#include "s60themeconvert.h"

int help()
{
    qDebug() << "Usage: s60theme [modeldir|theme.tdf] output.blob";
    qDebug() << "";
    qDebug() << "Options:";
    qDebug() << "   modeldir:    Theme 'model' directory in Carbide.ui tree";
    qDebug() << "   theme.tdf:   Theme .tdf file";
    qDebug() << "   output.blob: Theme blob output filename";
    qDebug() << "";
    qDebug() << "s60theme takes an S60 theme from Carbide.ui and converts";
    qDebug() << "it into a compact, binary format, that can be directly loaded by";
    qDebug() << "the QtS60Style.";
    qDebug() << "";
    qDebug() << "Visit http://www.forum.nokia.com for details about Carbide.ui";
    return 1;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
        return help();

    QApplication app(argc, argv);

    const QString input = QString::fromLatin1(argv[1]);
    const QFileInfo inputInfo(input);
    const QString output = QString::fromLatin1(argv[2]);
    if (inputInfo.isDir())
        return S60ThemeConvert::convertDefaultThemeToBlob(input, output) ? 0 : 1;
    else if (inputInfo.suffix().compare(QString::fromLatin1("tdf"), Qt::CaseInsensitive) == 0)
        return S60ThemeConvert::convertTdfToBlob(input, output) ? 0 : 1;

    return help();
}
