/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
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

#include "layoutitem.h"
#include "scene.h"

LayoutItem::LayoutItem(const QString &name, QGraphicsItem *parent/* = 0*/)
    : QGraphicsWidget(parent)
{
    setData(0, name);
    setGraphicsItem(this);
    setFocusPolicy(Qt::ClickFocus);
}

LayoutItem::~LayoutItem()
{
}

void LayoutItem::paint(QPainter *painter,
    const QStyleOptionGraphicsItem *option, QWidget *widget /*= 0*/)
{
    Q_UNUSED(widget);
    Q_UNUSED(option);

    // text
    QString name = graphicsItem()->data(0).toString();
    painter->drawText(rect(), Qt::AlignCenter, name);

    // rect
    if (Scene *scn = static_cast<Scene*>(scene())) {
        if (scn->lastFocusItem() == this) {
            QPen pen(Qt::blue, 2.0);
            painter->setPen(pen);
        }
    }
    painter->drawRoundedRect(rect(), 5, 5);
}

void LayoutItem::focusInEvent(QFocusEvent *event)
{
    if (Scene *scn = static_cast<Scene*>(scene())) {
        scn->emitFocusItemChanged(this);
    }
    QGraphicsWidget::focusInEvent(event);
}

void LayoutItem::focusOutEvent(QFocusEvent *event)
{
    if (Scene *scn = static_cast<Scene*>(scene())) {
        scn->emitFocusItemChanged(0);
    }
    QGraphicsWidget::focusOutEvent(event);
}


