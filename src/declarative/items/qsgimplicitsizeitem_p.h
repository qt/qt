// Commit: 6f78a6080b84cc3ef96b73a4ff58d1b5a72f08f4
/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QSGIMPLICITSIZEITEM_H
#define QSGIMPLICITSIZEITEM_H

#include "qsgpainteditem.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QSGImplicitSizeItemPrivate;
class Q_AUTOTEST_EXPORT QSGImplicitSizeItem : public QSGItem
{
    Q_OBJECT
    Q_PROPERTY(qreal implicitWidth READ implicitWidth NOTIFY implicitWidthChanged)
    Q_PROPERTY(qreal implicitHeight READ implicitHeight NOTIFY implicitHeightChanged)

public:
    QSGImplicitSizeItem(QSGItem *parent = 0);

protected:
    QSGImplicitSizeItem(QSGImplicitSizeItemPrivate &dd, QSGItem *parent);

Q_SIGNALS:
    void implicitWidthChanged();
    void implicitHeightChanged();

private:
    Q_DISABLE_COPY(QSGImplicitSizeItem)
    Q_DECLARE_PRIVATE(QSGImplicitSizeItem)
};

class QSGImplicitSizePaintedItemPrivate;
class Q_AUTOTEST_EXPORT QSGImplicitSizePaintedItem : public QSGPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(qreal implicitWidth READ implicitWidth NOTIFY implicitWidthChanged)
    Q_PROPERTY(qreal implicitHeight READ implicitHeight NOTIFY implicitHeightChanged)

public:
    QSGImplicitSizePaintedItem(QSGItem *parent = 0);

protected:
    QSGImplicitSizePaintedItem(QSGImplicitSizePaintedItemPrivate &dd, QSGItem *parent);
    virtual void drawContents(QPainter *, const QRect &) {};

Q_SIGNALS:
    void implicitWidthChanged();
    void implicitHeightChanged();

private:
    Q_DISABLE_COPY(QSGImplicitSizePaintedItem)
    Q_DECLARE_PRIVATE(QSGImplicitSizePaintedItem)
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QSGIMPLICITSIZEITEM_H
