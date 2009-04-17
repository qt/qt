/****************************************************************************
**
** Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the $MODULE$ of the Qt Toolkit.
**
** $TROLLTECH_DUAL_LICENSE$
**
****************************************************************************/

#include <QtCore/qobject.h>
#include <QtGui/qbrush.h>
#ifdef QT_EXPERIMENTAL_SOLUTION
#include "qtgraphicswidget.h"
#else
#include <QtGui/qgraphicswidget.h>
#endif

QT_BEGIN_NAMESPACE
class QGraphicsProxyWidget;
QT_END_NAMESPACE;

class RoundRectItem : public QGraphicsWidget
{
    Q_OBJECT
public:
    RoundRectItem(const QRectF &rect, const QBrush &brush, QWidget *embeddedWidget = 0);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *);
    QRectF boundingRect() const;

    void setPixmap(const QPixmap &pixmap);

Q_SIGNALS:
    void activated();

protected:
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

private:
    QRectF m_rect;
    QBrush brush;
    QPixmap pix;
    QGraphicsProxyWidget *proxyWidget;
};
