/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Linguist of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "testlupdate.h"
#include <stdlib.h>
#include <QtGui/QApplication>
#include <QtCore/QProcess>
#include <QtCore/QTimer>
#include <QtCore/QDir>

#ifdef Q_OS_WIN32
#   include <windows.h>
#endif

#include <QtTest/QtTest>


TestLUpdate::TestLUpdate()
{
    childProc = 0;
    QString binPath = QLibraryInfo::location(QLibraryInfo::BinariesPath);
    m_cmdLupdate = binPath + QLatin1String("/lupdate");
    m_cmdQMake = binPath + QLatin1String("/qmake");
}

TestLUpdate::~TestLUpdate()
{
    if (childProc)
        delete childProc;
}

void TestLUpdate::setWorkingDirectory(const QString &workDir)
{
    m_workDir = workDir;
    QDir::setCurrent(m_workDir);
}

void TestLUpdate::addMakeResult( const QString &result )
{
    make_result.append( result );
}

bool TestLUpdate::runChild( bool showOutput, const QString &program, const QStringList &argList)
{
    make_result.clear();
    exit_ok = FALSE;
    if (childProc)
        delete childProc;

    child_show = showOutput;
    if ( showOutput ) {
        QString S = argList.join(" ");
        addMakeResult( program + QLatin1String(" ") + S );
    }

    childProc = new QProcess();
    Q_ASSERT(childProc);

    childProc->setWorkingDirectory(m_workDir);
    connect(childProc, SIGNAL(finished(int)), this, SLOT(childReady(int)));
    childProc->setProcessChannelMode(QProcess::MergedChannels);
    if (argList.isEmpty()) {
        childProc->start( program );
    } else {
        childProc->start( program, argList );
    }
    bool ok;

    ok = childProc->waitForStarted();

    if (ok)
        ok = childProc->waitForFinished();

    if (!ok)
        addMakeResult( "Error executing '" + program + "'." );

    childReady(ok ? 0 : -1);

    return ok;
}

void TestLUpdate::childReady(int /*exitCode*/)
{
    if (childProc != 0) {
        childHasData();
        exit_ok = childProc->state() == QProcess::NotRunning
                  && childProc->exitStatus() == 0;
        childProc->deleteLater();
    }
    childProc = 0;
}

void TestLUpdate::childHasData()
{
    //QByteArray ba = childProc->readAllStandardError();
    //qDebug() << "ERROR:" << ba;
    QString stdoutput = childProc->readAllStandardOutput();
    stdoutput = stdoutput.replace("\t", "    ");
    if (child_show)
        addMakeResult(stdoutput);
}

bool TestLUpdate::run(const QString &commandline)
{
    return runChild(true, m_cmdLupdate + QLatin1String(" ") + commandline);
}


bool TestLUpdate::updateProFile(const QString &arguments)
{
    QStringList args = arguments.split(QChar(' '));
    return runChild( true, m_cmdLupdate, args );
}

bool TestLUpdate::qmake()
{
    QStringList args;
    args << "-r";
    return runChild(true, m_cmdQMake, args);
}
