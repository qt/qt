/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtSCriptTools module of the Qt Toolkit.
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

#include "qscriptdebuggerconsolecommandgroupdata_p.h"

#include <QtCore/qstring.h>

QT_BEGIN_NAMESPACE

/*!
  \since 4.5
  \class QScriptDebuggerConsoleCommandGroupData
  \internal

  \brief The QScriptDebuggerConsoleCommandGroupData class holds data associated with a console command group.
*/

class QScriptDebuggerConsoleCommandGroupDataPrivate
{
public:
    QScriptDebuggerConsoleCommandGroupDataPrivate();
    ~QScriptDebuggerConsoleCommandGroupDataPrivate();

    QString shortDescription;
    QString longDescription;

    QBasicAtomicInt ref;
};

QScriptDebuggerConsoleCommandGroupDataPrivate::QScriptDebuggerConsoleCommandGroupDataPrivate()
{
    ref = 0;
}

QScriptDebuggerConsoleCommandGroupDataPrivate::~QScriptDebuggerConsoleCommandGroupDataPrivate()
{
}

QScriptDebuggerConsoleCommandGroupData::QScriptDebuggerConsoleCommandGroupData()
    : d_ptr(0)
{
}

QScriptDebuggerConsoleCommandGroupData::QScriptDebuggerConsoleCommandGroupData(
    const QString &shortDescription, const QString &longDescription)
    : d_ptr(new QScriptDebuggerConsoleCommandGroupDataPrivate)
{
    d_ptr->shortDescription = shortDescription;
    d_ptr->longDescription = longDescription;
    d_ptr->ref.ref();
}

QScriptDebuggerConsoleCommandGroupData::QScriptDebuggerConsoleCommandGroupData(
    const QScriptDebuggerConsoleCommandGroupData &other)
    : d_ptr(other.d_ptr.data())
{
    if (d_ptr)
        d_ptr->ref.ref();
}

QScriptDebuggerConsoleCommandGroupData::~QScriptDebuggerConsoleCommandGroupData()
{
}

QScriptDebuggerConsoleCommandGroupData &QScriptDebuggerConsoleCommandGroupData::operator=(
    const QScriptDebuggerConsoleCommandGroupData &other)
{
    d_ptr.assign(other.d_ptr.data());
    return *this;
}

QString QScriptDebuggerConsoleCommandGroupData::shortDescription() const
{
    Q_D(const QScriptDebuggerConsoleCommandGroupData);
    return d->shortDescription;
}

QString QScriptDebuggerConsoleCommandGroupData::longDescription() const
{
    Q_D(const QScriptDebuggerConsoleCommandGroupData);
    return d->longDescription;
}

bool QScriptDebuggerConsoleCommandGroupData::isValid() const
{
    Q_D(const QScriptDebuggerConsoleCommandGroupData);
    return (d != 0);
}

QT_END_NAMESPACE
