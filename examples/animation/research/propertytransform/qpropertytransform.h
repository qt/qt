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

#include <QtCore/QObject>
#include <QtCore/QPoint>
#include <QtGui/QGraphicsItem>
#include <QtGui/QTransform>
#include <QtCore/QDebug>

/**
 * Experimental.
 * Pros:
 *          1. Does not add ugly/confusing API to QGraphicsItem.
 *
 * Cons:
 *          1. apply() /m_item->setTransform() is called too many times. (FIXED NOW?)
 *
 * 
 */
class QPropertyTransform : public QObject {
    Q_OBJECT
public:
    Q_PROPERTY(QPointF center READ center WRITE setCenter);
    Q_PROPERTY(qreal rotation READ rotation WRITE setRotation);
    Q_PROPERTY(qreal scaleX READ scaleX WRITE setScaleX);
    Q_PROPERTY(qreal scaleY READ scaleY WRITE setScaleY);
public:
    QPropertyTransform() : m_item(0), m_rotationZ(0), m_scaleX(1.), m_scaleY(1.) {}

    void setTargetItem(QGraphicsItem *item) {
        m_item = item;
    }

    void setCenter(const QPointF &center) {
        m_center = center;
        apply();
    }

    QPointF center() const { return m_center; }

    void setRotation(qreal rotation) {
        m_rotationZ = rotation;
        apply();
    }

    qreal rotation() const { return m_rotationZ; }

    void setScaleX(qreal scale) {
        m_scaleX = scale;
        apply();
    }

    qreal scaleX() const { return m_scaleX; }

    void setScaleY(qreal scale) {
        m_scaleY = scale;
        apply();
    }

    qreal scaleY() const { return m_scaleY; }
    
private:
    QTransform transform() const {
        return QTransform().translate(m_center.x(), m_center.y())
                         .rotate(m_rotationZ)
                         .scale(m_scaleX, m_scaleY)
                         .translate(-m_center.x(), -m_center.y());
    }

    void apply() {
        if (m_item)
            m_item->setTransform(transform());
    }

    QGraphicsItem *m_item;
    QPointF m_center;
    qreal m_rotationZ;
    qreal m_scaleX;
    qreal m_scaleY;
};

