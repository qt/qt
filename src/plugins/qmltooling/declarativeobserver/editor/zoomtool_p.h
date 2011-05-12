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

#ifndef ZOOMTOOL_H
#define ZOOMTOOL_H

#include "abstractliveedittool_p.h"
#include "liverubberbandselectionmanipulator_p.h"

QT_FORWARD_DECLARE_CLASS(QAction)

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class ZoomTool : public AbstractLiveEditTool
{
    Q_OBJECT

public:
    enum ZoomDirection {
        ZoomIn,
        ZoomOut
    };

    explicit ZoomTool(QDeclarativeViewObserver *view);

    virtual ~ZoomTool();

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);

    void hoverMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *keyEvent);
    void itemsAboutToRemoved(const QList<QGraphicsItem*> &itemList);

    void clear();

protected:
    void selectedItemsChanged(const QList<QGraphicsItem*> &itemList);

private slots:
    void zoomTo100();
    void zoomIn();
    void zoomOut();

private:
    qreal nextZoomScale(ZoomDirection direction) const;
    void scaleView(const QPointF &centerPos);

private:
    bool m_dragStarted;
    QPoint m_mousePos; // in view coords
    QPointF m_dragBeginPos;
    QAction *m_zoomTo100Action;
    QAction *m_zoomInAction;
    QAction *m_zoomOutAction;
    LiveRubberBandSelectionManipulator *m_rubberbandManipulator;

    qreal m_smoothZoomMultiplier;
    qreal m_currentScale;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // ZOOMTOOL_H
