/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef LAYEREDRENDERER_H
#define LAYEREDRENDERER_H

#include "renderer.h"
#include "material.h"

#include <qsgarray.h>
#include <QLinkedList>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class LayeredRenderer : public Renderer
{
public:
    LayeredRenderer();

    void render();

    void nodeChanged(Node *node, Node::DirtyFlags flags);

    bool allowRenderOrderUpdates() const { return true; }

private:
    typedef QLinkedList<GeometryNode *> NodeList;

    void mergeAndDraw(NodeList::const_iterator first, NodeList::const_iterator last);

    friend class RenderListBuilder;
    void renderNodes(const QLinkedList<GeometryNode *> &nodes);

    QLinkedList<GeometryNode *> m_nodes;

    uint m_nodes_require_sorting : 1;

    int m_lowest_render_order;
    int m_highest_render_order;

    QSGArray<float> m_float_vertices;
    QSGArray<quint16> m_indices;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // LAYEREDRENDERER_H
