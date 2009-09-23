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

#ifndef TESTLUPDATE_H
#define TESTLUPDATE_H

#include <QObject>
#include <QProcess>
#include <QStringList>

class TestLUpdate : public QObject
{
    Q_OBJECT

public:
    TestLUpdate();
    virtual ~TestLUpdate();

    void setWorkingDirectory( const QString &workDir);
    bool run( const QString &commandline);
    bool updateProFile( const QString &arguments);
    bool qmake();
    QStringList getErrorMessages() {
        return make_result;
    }
    void clearResult() {
        make_result.clear();
    }
private:
    QString     m_cmdLupdate;
    QString     m_cmdQMake;
    QString     m_workDir;
    QProcess	*childProc;
    QStringList env_list;
    QStringList make_result;

    bool	child_show;
    bool    qws_mode;
    bool	exit_ok;

    bool runChild( bool showOutput, const QString &program, const QStringList &argList = QStringList());
    void addMakeResult( const QString &result );
    void childHasData();

private slots:
    void childReady(int exitCode);
};

#endif // TESTLUPDATE_H
