/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QMLGRAPHICSFLIPABLE_H
#define QMLGRAPHICSFLIPABLE_H

#include "qmlgraphicsitem.h"

#include <QtCore/QObject>
#include <QtGui/QTransform>
#include <QtGui/qvector3d.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QmlGraphicsFlipablePrivate;
class Q_DECLARATIVE_EXPORT QmlGraphicsFlipable : public QmlGraphicsItem
{
    Q_OBJECT

    Q_ENUMS(Side)
    Q_PROPERTY(QmlGraphicsItem *front READ front WRITE setFront)
    Q_PROPERTY(QmlGraphicsItem *back READ back WRITE setBack)
    Q_PROPERTY(Side side READ side NOTIFY sideChanged)
    //### flipAxis
    //### flipRotation
public:
    QmlGraphicsFlipable(QmlGraphicsItem *parent=0);
    ~QmlGraphicsFlipable();

    QmlGraphicsItem *front();
    void setFront(QmlGraphicsItem *);

    QmlGraphicsItem *back();
    void setBack(QmlGraphicsItem *);

    enum Side { Front, Back };
    Side side() const;

Q_SIGNALS:
    void sideChanged();

private:
    Q_DISABLE_COPY(QmlGraphicsFlipable)
    Q_DECLARE_PRIVATE_D(QGraphicsItem::d_ptr.data(), QmlGraphicsFlipable)
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QmlGraphicsFlipable)

QT_END_HEADER

#endif // QMLGRAPHICSFLIPABLE_H
