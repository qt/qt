/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "liveselectionindicator.h"

#include "../qdeclarativeviewinspector_p.h"
#include "../qmlinspectorconstants.h"

#include <QtGui/QGraphicsRectItem>
#include <QtGui/QGraphicsObject>
#include <QtGui/QGraphicsScene>
#include <QtGui/QPen>

namespace QmlJSDebugger {

LiveSelectionIndicator::LiveSelectionIndicator(QDeclarativeViewInspector *viewInspector,
                                               QGraphicsObject *layerItem)
    : m_layerItem(layerItem)
    , m_view(viewInspector)
{
}

LiveSelectionIndicator::~LiveSelectionIndicator()
{
    clear();
}

void LiveSelectionIndicator::show()
{
    foreach (QGraphicsRectItem *item, m_indicatorShapeHash)
        item->show();
}

void LiveSelectionIndicator::hide()
{
    foreach (QGraphicsRectItem *item, m_indicatorShapeHash)
        item->hide();
}

void LiveSelectionIndicator::clear()
{
    if (!m_layerItem.isNull()) {
        QGraphicsScene *scene = m_layerItem.data()->scene();
        foreach (QGraphicsRectItem *item, m_indicatorShapeHash) {
            scene->removeItem(item);
            delete item;
        }
    }

    m_indicatorShapeHash.clear();

}

void LiveSelectionIndicator::setItems(const QList<QWeakPointer<QGraphicsObject> > &itemList)
{
    clear();

    foreach (const QWeakPointer<QGraphicsObject> &object, itemList) {
        if (object.isNull())
            continue;

        QGraphicsItem *item = object.data();

        if (!m_indicatorShapeHash.contains(item)) {
            QGraphicsRectItem *selectionIndicator = new QGraphicsRectItem(m_layerItem.data());
            m_indicatorShapeHash.insert(item, selectionIndicator);

            const QRectF boundingRect = m_view->adjustToScreenBoundaries(item->mapRectToScene(item->boundingRect()));
            const QRectF boundingRectInLayerItemSpace = m_layerItem.data()->mapRectFromScene(boundingRect);

            selectionIndicator->setData(Constants::EditorItemDataKey, true);
            selectionIndicator->setFlag(QGraphicsItem::ItemIsSelectable, false);
            selectionIndicator->setRect(boundingRectInLayerItemSpace);
            selectionIndicator->setPen(QColor(108, 141, 221));
        }
    }
}

} //namespace QmlJSDebugger

