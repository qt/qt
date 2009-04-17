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

#include <QtGui/qgraphicsview.h>
#ifdef QT_EXPERIMENTAL_SOLUTION
#include "qtgraphicswidget.h"
#else
#include <QtGui/qgraphicswidget.h>
#endif

QT_BEGIN_NAMESPACE
class Ui_BackSide;
QT_END_NAMESPACE;

class RoundRectItem;
class QAnimationGroup;
class QPropertyAnimation;

class Panel : public QGraphicsView
{
    Q_OBJECT
public:
    Panel(int width, int height);
    ~Panel();

protected:
    void keyPressEvent(QKeyEvent *event);
    void resizeEvent(QResizeEvent *event);

private Q_SLOTS:
    void flip();

private:
    QPointF posForLocation(int index) const;

    QGraphicsWidget *selectionItem;
    QGraphicsWidget *baseItem;
    RoundRectItem *backItem;
    QGraphicsWidget *splash;
    int selectedIndex;

    QVector<QGraphicsItem*> grid;
    
    int width;
    int height;
    bool flipped;
    Ui_BackSide *ui;

    QAnimationGroup *flippingGroup;
    QPropertyAnimation *rotationXanim, *rotationYanim;
};
