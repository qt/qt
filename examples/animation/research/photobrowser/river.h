/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
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
