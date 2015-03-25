/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the test suite of the Qt Toolkit.
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
#include "framework.h"

#include <QFile>
#include <QFileInfo>
#include <QSettings>
#include <QStringList>
#include <QtDebug>

Framework::Framework()
    : qsettings(0)
{
}

Framework::Framework(const QString &file)
    : qsettings(0)
{
    load(file);
}

Framework::~Framework()
{
    delete qsettings;
    qsettings = 0;
}

QString Framework::basePath() const
{
    if (!qsettings)
        return QString();

    QFileInfo fi(qsettings->fileName());
    return fi.absolutePath();
}


QStringList Framework::suites() const
{
    if (!qsettings)
        return QStringList();

    QStringList tests = qsettings->childGroups();
    qDebug()<<"here suites "<<tests;
    tests.removeAll("General");
    tests.removeAll("Blacklist");
    return tests;
}


bool Framework::isTestBlacklisted(const QString &engineName,
                                  const QString &testcase) const
{
    return m_blacklist[engineName].contains(testcase);
}

bool Framework::isValid() const
{
    return qsettings;
}

void Framework::load(const QString &file)
{
    if (qsettings) {
        delete qsettings;
        qsettings = 0;
    }
    if (QFile::exists(file)) {
        qsettings = new QSettings(file, QSettings::IniFormat);
        qsettings->beginGroup(QString("Blacklist"));
        QStringList engines = qsettings->childKeys();
        foreach(QString engineName, engines) {
            QStringList testcases = qsettings->value(engineName).toStringList();
            m_blacklist.insert(engineName, testcases);
            qDebug()<<"Blacklists for "<<testcases;
        }
        qsettings->endGroup();
    }
}

QString Framework::outputDir() const
{
    qsettings->beginGroup("General");
    QString outputDirName = qsettings->value("outputDir").toString();
    qsettings->endGroup();
    return outputDirName;
}

QSettings * Framework::settings() const
{
    return qsettings;
}
