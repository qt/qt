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

#ifndef QFXFLOWVIEW_H
#define QFXFLOWVIEW_H

#include <QtDeclarative/QmlTimeLine>
#include <QtDeclarative/qfxitem.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QFxVisualItemModel;
class QFxFlowViewValue;
class QFxFlowViewAttached;
class Q_DECLARATIVE_EXPORT QFxFlowView : public QFxItem
{
    Q_OBJECT
    Q_PROPERTY(QFxVisualItemModel *model READ model WRITE setModel)
    Q_PROPERTY(int column READ columns WRITE setColumns);
    Q_PROPERTY(bool vertical READ vertical WRITE setVertical);

public:
    QFxFlowView();

    QFxVisualItemModel *model() const;
    void setModel(QFxVisualItemModel *);

    int columns() const;
    void setColumns(int);

    bool vertical() const;
    void setVertical(bool);

    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

    static QFxFlowViewAttached *qmlAttachedProperties(QObject *);

private:
    QRectF rectForItem(int idx) const;
    void refresh();
    void reflow(bool animate = false);
    void reflowDrag(const QPointF &);
    void clearTimeLine();
    int m_columns;
    QFxVisualItemModel *m_model;
    QList<QFxItem *> m_items;
    bool m_vertical;
    void moveItem(QFxItem *item, const QPointF &);

    QPointF m_dragOffset;
    QFxItem *m_dragItem;

    QmlTimeLine m_timeline;
    QList<QFxFlowViewValue *> m_values;
    int m_dragIdx;
};
QML_DECLARE_TYPE(QFxFlowView);

QT_END_NAMESPACE

QT_END_HEADER

#endif // QFXFLOWVIEW_H
