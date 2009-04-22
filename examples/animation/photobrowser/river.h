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

#ifndef __RIVER__H__
#define __RIVER__H__

#include <QtCore/QDirIterator>
#include <QtGui/QGraphicsWidget>

#include "menu.h"

class RiverItem;

class River : public QGraphicsWidget
{
    Q_OBJECT
public:
    enum Mode
    {
        RiverMode,
        GridMode,
        GridFullScreenMode,
        CoverMode
    };

    River(const QString &path);
    void addUnusedRiverItem(RiverItem * item);

    static QSize fixedSize();

    void switchPaused();

    void setMode(Mode m);
    Mode mode() const;

    Menu *menu;

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void keyPressEvent ( QKeyEvent * keyEvent );
    void makeCenterAvailable(qreal size);
    QPointF gridItemPosition(int row, int col) const;
    QPixmap pixmap(int index) const;

protected slots:
    void animationFinished();
public slots:
    void setRiverMode();
    void setGridMode();
    void setCoverMode();

private:
    const QFileInfoList m_images;
    int m_currentImage;
    mutable QVector<QPixmap> m_pixmaps;
    QHash<int, RiverItem*> m_items;
    QList<RiverItem*> m_gridItem;
    QList<QItemAnimation*> m_pausedAnimations;
    Mode m_mode;
    
    void adjustGrid(int topRight);
    RiverItem *currentItem() { return m_items.value(m_selectedItem); }
    RiverItem *createItem(int imageIndex);
    void setCurrentItem(int currentItem);
    void navigateBy(int offset);
    
    int m_selectedItem;
    int m_topLeftIndex;

};


#endif //__RIVERITEM__H__
