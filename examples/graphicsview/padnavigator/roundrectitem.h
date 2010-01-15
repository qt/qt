/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
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

#include <QtCore/qobject.h>
#include <QtCore/qtimeline.h>
#include <QtGui/qgraphicsitem.h>
#include <QtGui/qbrush.h>

QT_BEGIN_NAMESPACE
class QGraphicsProxyWidget;
QT_END_NAMESPACE

class RoundRectItem : public QObject, public QGraphicsRectItem
{
    Q_OBJECT
public:
    RoundRectItem(const QRectF &rect, const QBrush &brush, QWidget *embeddedWidget = 0);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *);
    QRectF boundingRect() const;

    void setPixmap(const QPixmap &pixmap);

    qreal opacity() const;
    void setOpacity(qreal opacity);

Q_SIGNALS:
    void activated();

protected:
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

private slots:
    void updateValue(qreal value);

private:
    QBrush brush;
    QPixmap pix;
    QTimeLine timeLine;
    qreal lastVal;
    qreal opa;

    QGraphicsProxyWidget *proxyWidget;
};
