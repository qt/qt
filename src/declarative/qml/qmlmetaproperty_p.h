/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QMLMETAPROPERTY_P_H
#define QMLMETAPROPERTY_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qmlmetaproperty.h"

class QmlContext;
class QmlMetaPropertyPrivate
{
public:
    QmlMetaPropertyPrivate()
        : context(0), coreIdx(-1), type(QmlMetaProperty::Invalid), attachedFunc(-1), 
          object(0), propType(-1), category(QmlMetaProperty::Unknown) {}
    QmlMetaPropertyPrivate(const QmlMetaPropertyPrivate &other)
        : name(other.name), signal(other.signal), context(other.context), 
          coreIdx(other.coreIdx), type(other.type), attachedFunc(other.attachedFunc), 
          object(other.object), propType(other.propType),
          category(other.category) {}

    QString name;
    QMetaMethod signal;
    QmlContext *context;
    int coreIdx;
    uint type;
    int attachedFunc;
    QObject *object;
    int propType;

    mutable QmlMetaProperty::PropertyCategory category;

    QObject *attachedObject() const;
    void findSignalInt(QObject *, const QString &);

    int propertyType() const;
    QmlMetaProperty::PropertyCategory propertyCategory() const;

    void writeSignalProperty(const QVariant &);
    void writeValueProperty(const QVariant &);
};

#endif // QMLMETAPROPERTY_P_H
