/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Designer of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "abstractintegration.h"
#include "abstractformeditor.h"

#include <QtCore/QVariant>
#include <QtCore/QSharedPointer>

QT_BEGIN_NAMESPACE

// Add 'private' struct as a dynamic property.

static const char privatePropertyC[] = "_q_integrationprivate";

struct QDesignerIntegrationInterfacePrivate {
    QDesignerIntegrationInterfacePrivate() :
        headerSuffix(QLatin1String(".h")),
        headerLowercase(true) {}

    QString headerSuffix;
    bool headerLowercase;
};

typedef QSharedPointer<QDesignerIntegrationInterfacePrivate> QDesignerIntegrationInterfacePrivatePtr;

QT_END_NAMESPACE
Q_DECLARE_METATYPE(QT_PREPEND_NAMESPACE(QDesignerIntegrationInterfacePrivatePtr))
QT_BEGIN_NAMESPACE

static QDesignerIntegrationInterfacePrivatePtr integrationD(const QObject *o)
{
    const QVariant property = o->property(privatePropertyC);
    Q_ASSERT(qVariantCanConvert<QDesignerIntegrationInterfacePrivatePtr>(property));
    return qvariant_cast<QDesignerIntegrationInterfacePrivatePtr>(property);
}

QDesignerIntegrationInterface::QDesignerIntegrationInterface(QDesignerFormEditorInterface *core, QObject *parent)
    : QObject(parent),
      m_core(core)
{
    core->setIntegration(this);
    const QDesignerIntegrationInterfacePrivatePtr d(new QDesignerIntegrationInterfacePrivate);
    setProperty(privatePropertyC, qVariantFromValue<QDesignerIntegrationInterfacePrivatePtr>(d));
}

QString QDesignerIntegrationInterface::headerSuffix() const
{
    return integrationD(this)->headerSuffix;
}

void QDesignerIntegrationInterface::setHeaderSuffix(const QString &headerSuffix)
{
    integrationD(this)->headerSuffix = headerSuffix;
}

bool QDesignerIntegrationInterface::isHeaderLowercase() const
{
    return integrationD(this)->headerLowercase;
}

void QDesignerIntegrationInterface::setHeaderLowercase(bool headerLowercase)
{
    integrationD(this)->headerLowercase = headerLowercase;
}

QT_END_NAMESPACE
