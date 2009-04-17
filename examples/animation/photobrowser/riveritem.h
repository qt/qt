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

#ifndef __RIVERITEM__H__
#define __RIVERITEM__H__

#include <QtGui/QGraphicsPixmapItem>

class RiverItemAnimator;

class RiverItem : public QGraphicsPixmapItem
{
public:
    RiverItem(QGraphicsItem *parent);
    ~RiverItem();

    void setPixmap(const QPixmap &);
    void setFullScreen(bool b, qreal originScaleFactor);
    
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent*);

private:
    QPointF m_nonFSPos; //to save the position when not in fullscreen
    bool m_fullscreen;
};

#endif //__RIVERITEM__H__
