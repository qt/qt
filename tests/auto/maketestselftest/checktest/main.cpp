/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QStringList>

#include <stdio.h>
#include <stdlib.h>

void fail(QString const& message)
{
    printf("CHECKTEST FAIL: %s\n", qPrintable(message));
    exit(0);
}

void pass(QString const& message)
{
    printf("CHECKTEST PASS: %s\n", qPrintable(message));
    exit(0);
}

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    QStringList args = app.arguments();
    args.removeFirst(); // ourself

    QString args_quoted = QString("'%1'").arg(args.join("','"));

#ifdef Q_WS_QWS
    {
        // for QWS we expect tests to be run as the QWS server
        QString qws = args.takeLast();
        if (qws != "-qws") {
            fail(QString("Expected test to be run with `-qws', but it wasn't; args: %1").arg(args_quoted));
        }
    }
#endif

    if (args.count() != 1) {
        fail(QString("These arguments are not what I expected: %1").arg(args_quoted));
    }

    QString test = args.at(0);

    QFileInfo testfile(test);
    if (!testfile.exists()) {
        fail(QString("File %1 does not exist (my working directory is: %2, my args are: %3)")
            .arg(test)
            .arg(QDir::currentPath())
            .arg(args_quoted)
        );
    }

    pass(args_quoted);
}

