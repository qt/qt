/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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

#include "qmllistaccessor_p.h"

#include <qmlmetatype.h>

#include <QStringList>
#include <QtCore/qdebug.h>

// ### Remove me
#include <qmlengine_p.h>

QT_BEGIN_NAMESPACE

QmlListAccessor::QmlListAccessor()
: m_type(Invalid)
{
}

QmlListAccessor::~QmlListAccessor()
{
}

QVariant QmlListAccessor::list() const
{
    return d;
}

void QmlListAccessor::setList(const QVariant &v, QmlEngine *engine)
{
    d = v;

    QmlEnginePrivate *enginePrivate = engine?QmlEnginePrivate::get(engine):0;

    if (!d.isValid()) {
        m_type = Invalid;
    } else if (d.userType() == QVariant::StringList) {
        m_type = StringList;
    } else if (d.userType() == QMetaType::QVariantList) {
        m_type = VariantList;
    } else if (d.canConvert(QVariant::Int)) {
        m_type = Integer;
    } else if ((!enginePrivate && QmlMetaType::isQObject(d.userType())) ||
               (enginePrivate && enginePrivate->isQObject(d.userType()))) {
        QObject *data = enginePrivate?enginePrivate->toQObject(v):QmlMetaType::toQObject(v);
        d = QVariant::fromValue(data);
        m_type = Instance;
    } else if (d.userType() == qMetaTypeId<QmlListReference>()) {
        m_type = ListProperty;
    } else {
        m_type = Instance;
    }
}

int QmlListAccessor::count() const
{
    switch(m_type) {
    case StringList:
        return qvariant_cast<QStringList>(d).count();
    case VariantList:
        return qvariant_cast<QVariantList>(d).count();
    case ListProperty:
        return ((QmlListReference *)d.constData())->count();
    case Instance:
        return 1;
    case Integer:
        return d.toInt();
    default:
    case Invalid:
        return 0;
    }
}

QVariant QmlListAccessor::at(int idx) const
{
    Q_ASSERT(idx >= 0 && idx < count());
    switch(m_type) {
    case StringList:
        return QVariant::fromValue(qvariant_cast<QStringList>(d).at(idx));
    case VariantList:
        return qvariant_cast<QVariantList>(d).at(idx);
    case ListProperty:
        return QVariant::fromValue(((QmlListReference *)d.constData())->at(idx));
    case Instance:
        return d;
    case Integer:
        return QVariant(idx);
    default:
    case Invalid:
        return QVariant();
    }
}

bool QmlListAccessor::isValid() const
{
    return m_type != Invalid;
}

QT_END_NAMESPACE
