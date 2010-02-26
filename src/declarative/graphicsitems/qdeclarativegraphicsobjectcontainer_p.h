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

#ifndef QDECLARATIVEGRAPHICSOBJECTCONTAINER_H
#define QDECLARATIVEGRAPHICSOBJECTCONTAINER_H

#include "qdeclarativeitem.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QGraphicsObject;
class QDeclarativeGraphicsObjectContainerPrivate;

class Q_DECLARATIVE_EXPORT QDeclarativeGraphicsObjectContainer : public QDeclarativeItem
{
    Q_OBJECT

    Q_CLASSINFO("DefaultProperty", "graphicsObject")
    Q_PROPERTY(QGraphicsObject *graphicsObject READ graphicsObject WRITE setGraphicsObject)
    Q_PROPERTY(bool synchronizedResizing READ synchronizedResizing WRITE setSynchronizedResizing)

public:
    QDeclarativeGraphicsObjectContainer(QDeclarativeItem *parent = 0);
    ~QDeclarativeGraphicsObjectContainer();

    QGraphicsObject *graphicsObject() const;
    void setGraphicsObject(QGraphicsObject *);

    bool synchronizedResizing() const;
    void setSynchronizedResizing(bool on);

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    bool eventFilter(QObject *watched, QEvent *e);

private:
    Q_PRIVATE_SLOT(d_func(), void _q_updateSize())
    Q_DECLARE_PRIVATE_D(QGraphicsItem::d_ptr.data(), QDeclarativeGraphicsObjectContainer)
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QGraphicsObject)
QML_DECLARE_TYPE(QDeclarativeGraphicsObjectContainer)

QT_END_HEADER

#endif // QDECLARATIVEGRAPHICSOBJECTCONTAINER_H
