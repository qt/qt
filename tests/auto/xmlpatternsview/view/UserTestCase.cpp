/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QCoreApplication>
#include <QDate>
#include <QUrl>
#include <QVariant>
#include <QtDebug>

#include "TestResult.h"

#include "UserTestCase.h"

using namespace QPatternistSDK;

UserTestCase::UserTestCase() : m_lang(QXmlQuery::XQuery10)
{
}

QVariant UserTestCase::data(const Qt::ItemDataRole role, int /*column*/) const
{
    if(role != Qt::DisplayRole)
        return QVariant();

    return title();
}

QString UserTestCase::creator() const
{
    return QString(QLatin1String("The user of %1."))
                   .arg(QCoreApplication::instance()->applicationName());
}

QString UserTestCase::name() const
{
    return QString(QLatin1String("X-KDE-%1-UserCreated"))
                   .arg(QCoreApplication::instance()->applicationName());
}

QString UserTestCase::description() const
{
    return QLatin1String("No description available; the test case is not part of "
                         "a test suite, but entered manually in the source code window.");
}

QString UserTestCase::title() const
{
    return QLatin1String("User Specified Test");
}

TestCase::Scenario UserTestCase::scenario() const
{
    return Standard;
}

TestBaseLine::List UserTestCase::baseLines() const
{
    TestBaseLine::List retval;

    TestBaseLine *const bl = new TestBaseLine(TestBaseLine::Ignore);
    retval.append(bl);

    return retval;
}

void UserTestCase::setSourceCode(const QString &code)
{
    m_sourceCode = code;
}

QString UserTestCase::sourceCode(bool &ok) const
{
    ok = true;
    return m_sourceCode;
}

QDate UserTestCase::lastModified() const
{
    return QDate();
}

bool UserTestCase::isXPath() const
{
    return true;
}

TreeItem *UserTestCase::parent() const
{
    return 0;
}

int UserTestCase::columnCount() const
{
    return 1;
}

QUrl UserTestCase::testCasePath() const
{
    return QUrl::fromLocalFile(QCoreApplication::applicationDirPath());
}

QPatternist::ExternalVariableLoader::Ptr UserTestCase::externalVariableLoader() const
{
    /* We don't have any bindings for the query that the user writes. */
    return QPatternist::ExternalVariableLoader::Ptr();
}

QUrl UserTestCase::contextItemSource() const
{
    return m_contextSource;
}

void UserTestCase::focusDocumentChanged(const QString &code)
{
    const QUrl focusDoc(code);
    if(focusDoc.isValid())
        m_contextSource = focusDoc;
}

// vim: et:ts=4:sw=4:sts=4

