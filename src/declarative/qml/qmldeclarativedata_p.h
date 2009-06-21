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

#ifndef QMLDECLARATIVEDATA_P_H
#define QMLDECLARATIVEDATA_P_H

#include <private/qobject_p.h>

QT_BEGIN_NAMESPACE

class QmlSimpleDeclarativeData : public QDeclarativeData
{
public:
    QmlSimpleDeclarativeData() : flags(0), context(0) {}

    virtual void destroyed(QObject *);
    enum Flag { Instance = 0x00000001, Extended = 0x00000002 };
    quint32 flags;
    QmlContext *context;

    static inline QmlSimpleDeclarativeData *get(QObject *object, 
                                                bool create = false);
};

class QmlCompiledComponent;
class QmlInstanceDeclarativeData : public QmlSimpleDeclarativeData
{
public:
    QmlInstanceDeclarativeData() : deferredComponent(0) { flags |= Instance; }

    virtual void destroyed(QObject *);

    QmlCompiledComponent *deferredComponent;
    unsigned int deferredIdx;

    static inline QmlInstanceDeclarativeData *get(QObject *object, 
                                                  bool create = false);
};

class QmlExtendedDeclarativeData : public QmlInstanceDeclarativeData
{
public:
    QmlExtendedDeclarativeData() { flags |= Extended; }

    QHash<int, QObject *> attachedProperties;

    static inline QmlExtendedDeclarativeData *get(QObject *object, 
                                                  bool create = false);
};

QmlSimpleDeclarativeData *
QmlSimpleDeclarativeData::get(QObject *object, bool create)
{
    QObjectPrivate *priv = QObjectPrivate::get(object);

    if (create && !priv->declarativeData)
        priv->declarativeData = new QmlInstanceDeclarativeData;

    return static_cast<QmlSimpleDeclarativeData *>(priv->declarativeData);
}

QmlInstanceDeclarativeData *
QmlInstanceDeclarativeData::get(QObject *object, bool create)
{
    QObjectPrivate *priv = QObjectPrivate::get(object);

    QmlSimpleDeclarativeData *simple = 
        static_cast<QmlSimpleDeclarativeData *>(priv->declarativeData);

    if (simple && (simple->flags & Instance)) {
        return static_cast<QmlInstanceDeclarativeData *>(simple);
    } else if (create && simple) {
        QmlInstanceDeclarativeData *rv = new QmlInstanceDeclarativeData;
        rv->context = simple->context;
        simple->destroyed(object);
        priv->declarativeData = rv;
        return rv;
    } else if (create) {
        QmlInstanceDeclarativeData *rv = new QmlInstanceDeclarativeData;
        priv->declarativeData = rv;
        return rv;
    }
    return 0;
}

QmlExtendedDeclarativeData *
QmlExtendedDeclarativeData::get(QObject *object, bool create)
{
    QObjectPrivate *priv = QObjectPrivate::get(object);

    QmlSimpleDeclarativeData *simple = 
        static_cast<QmlSimpleDeclarativeData *>(priv->declarativeData);

    if (simple && (simple->flags & Extended)) {
        return static_cast<QmlExtendedDeclarativeData *>(simple);
    } else if (create && simple) {
        QmlExtendedDeclarativeData *rv = new QmlExtendedDeclarativeData;
        rv->context = simple->context;

        if (simple->flags & Instance) {
            QmlInstanceDeclarativeData *instance = 
                static_cast<QmlInstanceDeclarativeData *>(priv->declarativeData);
            rv->deferredComponent = instance->deferredComponent;
            rv->deferredIdx = instance->deferredIdx;
            delete simple;
        } else {
            simple->destroyed(object);
        }
        priv->declarativeData = rv;
        return rv;
    } else if (create) {
        QmlExtendedDeclarativeData *rv = new QmlExtendedDeclarativeData;
        priv->declarativeData = rv;
        return rv;
    }
    return 0;
}

QT_END_NAMESPACE

#endif // QMLDECLARATIVEDATA_P_H
