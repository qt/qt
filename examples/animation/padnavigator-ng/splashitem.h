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
#ifdef QT_EXPERIMENTAL_SOLUTION
#include "qtgraphicswidget.h"
#else
#include <QtGui/qgraphicswidget.h>
#endif

class SplashItem : public QGraphicsWidget
{
    Q_OBJECT
public:
    SplashItem(QGraphicsItem *parent = 0);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

protected:
    void keyPressEvent(QKeyEvent *event);

private:
    QString text;
};
