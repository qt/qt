/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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

#include "qfxvisualitemmodel.h"
#include "qfxflowview.h"

#include <QGraphicsSceneMouseEvent>

QT_BEGIN_NAMESPACE

class QFxFlowViewAttached : public QObject
{
Q_OBJECT
Q_PROPERTY(int row READ row NOTIFY posChanged);
Q_PROPERTY(int column READ column NOTIFY posChanged);
public:
    QFxFlowViewAttached(QObject *parent);

    int row() const;
    int column() const;

signals:
    void posChanged();

private:
    friend class QFxFlowView;
    int m_row;
    int m_column;
};


QFxFlowView::QFxFlowView()
: m_columns(0), m_model(0), m_vertical(false), m_dragItem(0), m_dragIdx(-1)
{
    setAcceptedMouseButtons(Qt::LeftButton);
    setOptions(MouseEvents);
}

QFxVisualItemModel *QFxFlowView::model() const
{
    return m_model;
}

void QFxFlowView::setModel(QFxVisualItemModel *m)
{
    m_model = m;
    refresh();
}

int QFxFlowView::columns() const
{
    return m_columns;
}

void QFxFlowView::setColumns(int c)
{
    m_columns = c;
    refresh();
}

bool QFxFlowView::vertical() const
{
    return m_vertical;
}

void QFxFlowView::setVertical(bool v)
{
    m_vertical = v;
}

class QFxFlowViewValue : public QmlTimeLineValue
{
public:
    enum Property { xProperty, yProperty };

    QFxFlowViewValue(QFxItem *item, Property p)
        : m_item(item), m_property(p) {}

    qreal value() const {
        return (m_property == xProperty)?m_item->x():m_item->y();
    }
    void setValue(qreal v) {
        if (m_property == xProperty) m_item->setX(v);
        else m_item->setY(v);
    }
private:
    QFxItem *m_item;
    Property m_property;
};

void QFxFlowView::refresh()
{
    if (m_model && m_columns >= 1) {
        for (int ii = 0; ii < m_model->count(); ++ii) {
            if (QFxItem *item = m_model->item(ii)) {
                item->setParent(this);
                item->setZ(0);
                m_items << item;
            }
        }

        reflow();
    }
}

void QFxFlowView::reflow(bool animate)
{
    qreal y = 0;
    qreal maxY = 0;
    qreal x = 0;
    int row = 0;
    int column = -1;
    if (animate)
        clearTimeLine();

    for (int ii = 0; ii < m_items.count(); ++ii) {
        if (0 == (ii % m_columns)) {
            y += maxY;
            maxY = 0;
            x = 0;
            row = 0;
            column++;
        }

        QFxItem *item = m_items.at(ii);
        QFxFlowViewAttached *att = 
            (QFxFlowViewAttached *)qmlAttachedPropertiesObject<QFxFlowView>(item);
        att->m_row = row;
        att->m_column = column;
        emit att->posChanged();


        if (animate) {
            if (vertical())
                moveItem(item, QPointF(y, x));
            else
                moveItem(item, QPointF(x, y));
        } else {
            if (vertical()) {
                item->setX(y);
                item->setY(x);
            } else {
                item->setX(x);
                item->setY(y);
            }
        }
        if (vertical()) {
            x += item->height();
            maxY = qMax(maxY, item->width());
        } else {
            x += item->width();
            maxY = qMax(maxY, item->height());
        }
        ++row;
    }
}

void QFxFlowView::reflowDrag(const QPointF &dp)
{
    qreal y = 0;
    qreal maxY = 0;
    qreal x = 0;

    clearTimeLine();

    QList<QFxItem *> items;

    bool dragUsed = false;
    bool dragSeen = false;
    for (int ii = 0; ii < m_items.count(); ++ii) {
        if (0 == (ii % m_columns)) {
            y += maxY;
            maxY = 0;
            x = 0;
        }

        QFxItem *item = m_items.at(ii);
        if (item == m_dragItem)
            dragSeen = true;
        if (item == m_dragItem && dragUsed)
            continue;
        QRectF r;
        if (vertical())
            r = QRectF(y, x, item->width(), item->height());
        else
            r = QRectF(x, y, item->width(), item->height());
        if (r.contains(dp)) {
            dragUsed = true;
            if (dragSeen)
                m_dragIdx = items.count() + 1;
            else
                m_dragIdx = items.count();

            items.append(m_dragItem);
            if (m_dragItem != item) {
                if (dragSeen)
                    items.insert(items.count() - 1, item);
                else 
                    items.append(item);
            }
        } else {
            if (m_dragItem != item)
                items.append(item);
        }
    
        if (vertical()) {
            x += item->height();
            maxY = qMax(maxY, item->width());
        } else {
            x += item->width();
            maxY = qMax(maxY, item->height());
        }
    }

    y = 0;
    maxY = 0;
    x = 0;
    clearTimeLine();
    for (int ii = 0; ii < items.count(); ++ii) {
        if (0 == (ii % m_columns)) {
            y += maxY;
            maxY = 0;
            x = 0;
        }

        QFxItem *item = items.at(ii);
        if (item != m_dragItem) {
            if (vertical())
                moveItem(item, QPointF(y, x));
            else
                moveItem(item, QPointF(x, y));
        }
        if (vertical()) {
            x += item->height();
            maxY = qMax(maxY, item->width());
        } else {
            x += item->width();
            maxY = qMax(maxY, item->height());
        }
    }

}

void QFxFlowView::moveItem(QFxItem *item, const QPointF &p)
{
    QFxFlowViewValue *xv = new QFxFlowViewValue(item, QFxFlowViewValue::xProperty);
    QFxFlowViewValue *yv = new QFxFlowViewValue(item, QFxFlowViewValue::yProperty);
    m_values << xv << yv;
    m_timeline.move(*xv, p.x(), 100);
    m_timeline.move(*yv, p.y(), 100);
}

void QFxFlowView::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    for (int ii = 0; ii < m_items.count(); ++ii) {
        QFxItem *item = m_items.at(ii);
        QRectF r = rectForItem(ii);

        if (r.contains(event->pos())) {
            m_dragItem = item;
            m_dragItem->setZ(1);
            m_dragOffset = r.topLeft() - event->pos();
            event->accept();
            return;
        }
    }
    event->ignore();
}

QRectF QFxFlowView::rectForItem(int idx) const
{
    QFxItem *item = m_items.at(idx);
    QRectF r(item->x(), item->y(), item->width(), item->height());
    return r;
}

void QFxFlowView::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);
    if (m_dragItem) {
        m_dragItem->setZ(0);

        clearTimeLine();

        if (m_dragIdx != -1) {
            m_items.removeAll(m_dragItem);
            m_items.insert(m_dragIdx, m_dragItem);
        }

        reflow(true);
        m_dragItem = 0;
        m_dragIdx = -1;
    }
}

QFxFlowViewAttached::QFxFlowViewAttached(QObject *parent)
: QObject(parent), m_row(0), m_column(0)
{
}

int QFxFlowViewAttached::row() const
{
    return m_row;
}

int QFxFlowViewAttached::column() const
{
    return m_column;
}

QFxFlowViewAttached *QFxFlowView::qmlAttachedProperties(QObject *obj)
{
    return new QFxFlowViewAttached(obj);
}

void QFxFlowView::clearTimeLine()
{
    m_timeline.clear();
    qDeleteAll(m_values);
    m_values.clear();
}

void QFxFlowView::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (m_dragItem) {
        QPointF p = event->pos() + m_dragOffset;
        m_dragItem->setX(p.x());
        m_dragItem->setY(p.y());

        for (int ii = 0; ii < m_items.count(); ++ii) {
            if (m_items.at(ii) != m_dragItem && rectForItem(ii).contains(event->pos())) {
                reflowDrag(event->pos());
            }
        }
    }
}

QML_DEFINE_TYPE(QFxFlowView,FlowView);

QT_END_NAMESPACE

#include "qfxflowview.moc"
