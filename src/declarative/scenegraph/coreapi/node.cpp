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

#include "node.h"
#include "renderer.h"
#include "nodeupdater_p.h"
#include "material.h"
#include <qglattributevalue.h>

#include "limits.h"

Node::Node()
    : m_parent(0)
    , m_nodeFlags(0)
    , m_flags(OwnedByParent)
    , m_updateFlags(0)
    , m_subtree_enabled(true)
{
}

Node::~Node()
{
    destroy();
}

void Node::destroy()
{
    if (m_parent) {
        m_parent->removeChildNode(this);
        Q_ASSERT(m_parent == 0);
    }
    for (int ii = m_children.count() - 1; ii >= 0; --ii) {
        Node *child = m_children.at(ii);
        removeChildNode(child);
        Q_ASSERT(child->m_parent == 0);
        if (child->flags() & OwnedByParent)
            delete child;
    }
    Q_ASSERT(m_children.isEmpty());
    qDeleteAll(m_data.values());
    m_data.clear();
}

RootNode *Node::root() const
{
    const Node *n = this;
    while (n && n->type() != Node::RootNodeType)
        n = n->m_parent;
    return (RootNode *) n;
}

void Node::prependChildNode(Node *node)
{
    Q_ASSERT_X(!m_children.contains(node), "Node::prependChildNode", "Node is already a child!");
    Q_ASSERT_X(!node->m_parent, "Node::prependChildNode", "Node already has a parent");
    Q_ASSERT_X(node->type() != RootNodeType, "Node::prependChildNode", "RootNodes cannot be children of other nodes");

#ifndef QT_NO_DEBUG
    if (node->type() == Node::GeometryNodeType) {
        GeometryNode *g = static_cast<GeometryNode *>(node);
        Q_ASSERT_X(g->material(), "Node::prependChildNode", "GeometryNode is missing material");
        Q_ASSERT_X(!g->geometry()->isNull(), "Node::prependChildNode", "GeometryNode is missing geometry");
    }
#endif

    m_children.prepend(node);
    node->m_parent = this;

    node->markDirty(DirtyNodeAdded);
}

void Node::appendChildNode(Node *node)
{
    Q_ASSERT_X(!m_children.contains(node), "Node::appendChildNode", "Node is already a child!");
    Q_ASSERT_X(!node->m_parent, "Node::appendChildNode", "Node already has a parent");
    Q_ASSERT_X(node->type() != RootNodeType, "Node::appendChildNode", "RootNodes cannot be children of other nodes");

#ifndef QT_NO_DEBUG
    if (node->type() == Node::GeometryNodeType) {
        GeometryNode *g = static_cast<GeometryNode *>(node);
        Q_ASSERT_X(g->material(), "Node::appendChildNode", "GeometryNode is missing material");
        Q_ASSERT_X(!g->geometry()->isNull(), "Node::appendChildNode", "GeometryNode is missing geometry");
    }
#endif

    m_children.append(node);
    node->m_parent = this;

    node->markDirty(DirtyNodeAdded);
}

void Node::insertChildNodeBefore(Node *node, Node *before)
{
    Q_ASSERT_X(!m_children.contains(node), "Node::insertChildNodeBefore", "Node is already a child!");
    Q_ASSERT_X(!node->m_parent, "Node::insertChildNodeBefore", "Node already has a parent");
    Q_ASSERT_X(node->type() != RootNodeType, "Node::insertChildNodeBefore", "RootNodes cannot be children of other nodes");

#ifndef QT_NO_DEBUG
    if (node->type() == Node::GeometryNodeType) {
        GeometryNode *g = static_cast<GeometryNode *>(node);
        Q_ASSERT_X(g->material(), "Node::insertChildNodeBefore", "GeometryNode is missing material");
        Q_ASSERT_X(!g->geometry()->isNull(), "Node::insertChildNodeBefore", "GeometryNode is missing geometry");
    }
#endif

    int idx = before?m_children.indexOf(before):-1;
    if (idx == -1)
        m_children.append(node);
    else
        m_children.insert(idx, node);
    node->m_parent = this;

    node->markDirty(DirtyNodeAdded);
}

void Node::insertChildNodeAfter(Node *node, Node *after)
{
    Q_ASSERT_X(!m_children.contains(node), "Node::insertChildNodeAfter", "Node is already a child!");
    Q_ASSERT_X(!node->m_parent, "Node::insertChildNodeAfter", "Node already has a parent");
    Q_ASSERT_X(node->type() != RootNodeType, "Node::insertChildNodeAfter", "RootNodes cannot be children of other nodes");

#ifndef QT_NO_DEBUG
    if (node->type() == Node::GeometryNodeType) {
        GeometryNode *g = static_cast<GeometryNode *>(node);
        Q_ASSERT_X(g->material(), "Node::insertChildNodeAfter", "GeometryNode is missing material");
        Q_ASSERT_X(!g->geometry()->isNull(), "Node::insertChildNodeAfter", "GeometryNode is missing geometry");
    }
#endif

    int idx = after?m_children.indexOf(after):-1;
    if (idx == -1)
        m_children.append(node);
    else
        m_children.insert(idx + 1, node);
    node->m_parent = this;

    node->markDirty(DirtyNodeAdded);
}

void Node::removeChildNode(Node *node)
{
    Q_ASSERT(m_children.contains(node));
    Q_ASSERT(node->parent() == this);

    m_children.removeOne(node);

    node->markDirty(DirtyNodeRemoved);
    node->m_parent = 0;
}


void Node::setSubtreeEnabled(bool enabled)
{
    if (m_subtree_enabled == enabled)
        return;
    m_subtree_enabled = enabled;
    markDirty(DirtySubtreeEnabled);
}

void Node::setSubtreeRenderOrder(int order)
{
    for (int i=0; i<m_children.size(); ++i)
        m_children.at(i)->setSubtreeRenderOrder(order);
}

void Node::setFlag(Flag f, bool enabled)
{
    if (enabled)
        m_nodeFlags |= f;
    else
        m_nodeFlags &= ~f;
}

void Node::markDirty(DirtyFlags flags)
{
    m_flags |= (flags & DirtyPropagationMask);

    DirtyFlags subtreeFlags = DirtyFlags((flags & DirtyPropagationMask) << 16);
    Node *root = this;
    Node *p = m_parent;
    while (p) {
        p->m_flags |= subtreeFlags;
        root = p;
        p = p->m_parent;
    }

    if (root->type() == RootNodeType)
        static_cast<RootNode *>(root)->notifyNodeChange(this, flags);
}

QRectF Node::subtreeBoundingRect() const
{
    QRectF bounds;
    for (int i = 0; i < m_children.size(); ++i)
        bounds |= m_children.at(i)->subtreeBoundingRect();
    return bounds;
}


BasicGeometryNode::BasicGeometryNode()
    : m_first_index(0)
    , m_end_index(-1)
    , m_matrix(0)
    , m_clip_list(0)
{
    m_geometry = new Geometry();
}

BasicGeometryNode::~BasicGeometryNode()
{
    destroy();
    delete m_geometry;
}

void BasicGeometryNode::setGeometry(Geometry *geometry, int firstIndex, int endIndex)
{
    delete m_geometry;
    m_geometry = geometry;
    m_first_index = firstIndex;
    m_end_index = endIndex;
    markDirty(DirtyGeometry);
}

void BasicGeometryNode::updateGeometryDescription(const QVector<QGLAttributeDescription> &description, GLenum indexType)
{
    m_geometry->setIndexType(indexType);
    m_geometry->setVertexDescription(description);
    m_first_index = 0;
    m_end_index = -1;
    markDirty(DirtyGeometry);
}

void BasicGeometryNode::setFirstIndex(int index)
{
    m_first_index = index;
    markDirty(DirtyGeometry);
}

void BasicGeometryNode::setEndIndex(int index)
{
    m_end_index = index;
    markDirty(DirtyGeometry);
}

QPair<int, int> BasicGeometryNode::indexRange() const
{
    if (m_end_index >= 0)
        return QPair<int, int>(m_first_index, m_end_index);
    else if (m_geometry->indexCount())
        return QPair<int, int>(m_first_index, m_geometry->indexCount());
    else
        return QPair<int, int>(m_first_index, m_geometry->vertexCount());
}

//void BasicGeometryNode::setUsagePattern(UsagePattern pattern)
//{
//    // Skip this functionallity for now...
//    Q_UNUSED(pattern);
//}

void BasicGeometryNode::setBoundingRect(const QRectF &bounds)
{
    m_bounding_rect = bounds;
    markDirty(DirtyBoundingRect);
}



GeometryNode::GeometryNode()
    : m_render_order(0)
    , m_material(0)
{
}

GeometryNode::~GeometryNode()
{
    destroy();
}

/*!
    Sets the render order of this node to be \a order.

    GeometryNodes are rendered in an order that visually looks like
    low order nodes are rendered prior to high order nodes. For opaque
    geometry there is little difference as z-testing will handle
    the discard, but for translucent objects, the rendering should
    normally be specified in the order of back-to-front.

    The default render order is 0.
  */
void GeometryNode::setRenderOrder(int order)
{
    if (m_render_order != order) {
        m_render_order = order;
        markDirty(DirtyRenderOrder);
    }
}

void GeometryNode::setSubtreeRenderOrder(int order)
{
    setRenderOrder(order);
    Node::setSubtreeRenderOrder(order);
}

void GeometryNode::setMaterial(AbstractEffect *material)
{
    RootNode *r = root();
    if (r)
        r->notifyMaterialChange(this, m_material, material);
    m_material = material;
    markDirty(DirtyMaterial);
}

QRectF GeometryNode::subtreeBoundingRect() const
{
    return BasicGeometryNode::subtreeBoundingRect() | boundingRect();
}

ClipNode::ClipNode()
{
}

ClipNode::~ClipNode()
{
    destroy();
}

QRectF ClipNode::subtreeBoundingRect() const
{
    return BasicGeometryNode::subtreeBoundingRect() & boundingRect();
}


TransformNode::TransformNode()
{
}

TransformNode::~TransformNode()
{
    destroy();
}

void TransformNode::setMatrix(const QMatrix4x4 &matrix)
{
    m_matrix = matrix;
    markDirty(DirtyMatrix);
}

void TransformNode::setZ(qreal z)
{
    QMatrix4x4 m;
    m.translate(0, 0, z);
    setMatrix(m);
}

void TransformNode::translate(qreal dx, qreal dy, qreal dz)
{
    m_matrix.translate(dx, dy, dz);
    setMatrix(m_matrix);
}

void TransformNode::scale(qreal factor)
{
    m_matrix.scale(factor);
    setMatrix(m_matrix);
}

void TransformNode::scale(qreal x, qreal y, qreal z)
{
    m_matrix.scale(x, y, z);
    setMatrix(m_matrix);
}

void TransformNode::rotate(qreal angle, qreal x, qreal y, qreal z)
{
    m_matrix.rotate(angle, x, y, z);
    setMatrix(m_matrix);
}

QRectF TransformNode::subtreeBoundingRect() const
{
    return m_matrix.mapRect(Node::subtreeBoundingRect());
}



RootNode::~RootNode()
{
    while (!m_renderers.isEmpty())
        m_renderers.last()->setRootNode(0);
    destroy();
}

void RootNode::removeRenderer(Renderer *r)
{
    m_renderers.removeAll(r);
}


void RootNode::notifyNodeChange(Node *node, DirtyFlags flags)
{
    for (int i=0; i<m_renderers.size(); ++i)
        m_renderers.at(i)->nodeChanged(node, flags);
}

void RootNode::notifyMaterialChange(GeometryNode *node, AbstractEffect *from, AbstractEffect *to)
{
    for (int i=0; i<m_renderers.size(); ++i)
        m_renderers.at(i)->materialChanged(node, from, to);
}

void RootNode::updateDirtyStates()
{
    NodeUpdater updater;
    updater.visitNode(this);
}


NodeVisitor::~NodeVisitor()
{

}


void NodeVisitor::visitNode(Node *n)
{
    switch (n->type()) {
    case Node::TransformNodeType: {
        TransformNode *t = static_cast<TransformNode *>(n);
        enterTransformNode(t);
        visitChildren(t);
        leaveTransformNode(t);
        break; }
    case Node::GeometryNodeType: {
        GeometryNode *g = static_cast<GeometryNode *>(n);
        enterGeometryNode(g);
        visitChildren(g);
        leaveGeometryNode(g);
        break; }
    case Node::ClipNodeType: {
        ClipNode *c = static_cast<ClipNode *>(n);
        enterClipNode(c);
        visitChildren(c);
        leaveClipNode(c);
        break; }
    default:
        visitChildren(n);
        break;
    }
}

void NodeVisitor::visitChildren(Node *n)
{
    int count = n->childCount();
    for (int i=0; i<count; ++i) {
        visitNode(n->childAtIndex(i));
    }
}



#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug d, const GeometryNode *n)
{
    if (!n) {
        d << "GeometryNode(null)";
        return d;
    }
    d << "GeometryNode(" << hex << (void *) n << dec;

    const Geometry *g = n->geometry();
    switch (g->drawingMode()) {
    case QGL::TriangleStrip: d << "strip"; break;
    case QGL::TriangleFan: d << "fan"; break;
    case QGL::Triangles: d << "triangles"; break;
    default: break;
    }

     d << g->vertexCount();

    if (n->childCount())
        d << "children=" << n->childCount();
    if (n->material())
        d << "effect=" << n->material() << "type=" << n->material()->type();
    d << "order=" << n->renderOrder();

    QGLAttributeValue a = g->attributeValue(QGL::Position);
    if (!a.isNull() && a.type() == GL_FLOAT) {
        float x1 = 1e10, x2 = -1e10, y1=1e10, y2=-1e10;
        int stride = a.stride();
        if (stride == 0)
            stride = a.tupleSize() * a.sizeOfType();
        for (int i = 0; i < g->vertexCount(); ++i) {
            float x = ((float *)((char *)a.data() + i * stride))[0];
            float y = ((float *)((char *)a.data() + i * stride))[1];

            x1 = qMin(x1, x);
            x2 = qMax(x2, x);
            y1 = qMin(y1, y);
            y2 = qMax(y2, y);
        }

        d << "x1=" << x1 << "y1=" << y1 << "x2=" << x2 << "y2=" << y2;
    }
    d << "dirty=" << hex << (int) n->dirtyFlags() << dec;
    d << ")";
#ifdef QML_RUNTIME_TESTING
    d << n->description;
#endif
    return d;
}

QDebug operator<<(QDebug d, const ClipNode *n)
{
    if (!n) {
        d << "ClipNode(null)";
        return d;
    }
    d << "ClipNode(" << hex << (void *) n << dec;

    if (n->childCount())
        d << "children=" << n->childCount();

    d << "bbox=" << n->boundingRect();
    d << "is rect?" << (n->flags() & Node::ClipIsRectangular ? "yes" : "no");
    d << "dirty=" << hex << (int) n->dirtyFlags() << dec;

    d << ")";
#ifdef QML_RUNTIME_TESTING
    d << n->description;
#endif
    return d;
}

QDebug operator<<(QDebug d, const TransformNode *n)
{
    if (!n) {
        d << "TransformNode(null)";
        return d;
    }
    const QMatrix4x4 m = n->matrix();
    d << "TransformNode(";
    d << hex << (void *) n << dec;
    if (m.isIdentity())
        d << "identity";
    else if (m.determinant() == 1 && m(0, 0) == 1 && m(1, 1) == 1 && m(2, 2) == 1)
        d << "translate" << m(0, 3) << m(1, 3) << m(2, 3);
    else
        d << "det=" << n->matrix().determinant();
#ifdef QML_RUNTIME_TESTING
    d << n->description;
#endif
    d << "dirty=" << hex << (int) n->dirtyFlags() << dec;
    d << ")";
    return d;
}

QDebug operator<<(QDebug d, const Node *n)
{
    if (!n) {
        d << "Node(null)";
        return d;
    }
    switch (n->type()) {
    case Node::GeometryNodeType:
        d << static_cast<const GeometryNode *>(n);
        break;
    case Node::TransformNodeType:
        d << static_cast<const TransformNode *>(n);
        break;
    case Node::ClipNodeType:
        d << static_cast<const ClipNode *>(n);
        break;
    default:
        d << "Node(" << hex << (void *) n << dec << "children=" << n->childCount();
#ifdef QML_RUNTIME_TESTING
        d << n->description;
#endif
        d << "dirty=" << hex << (int) n->dirtyFlags() << dec;
        d << ")";
        break;
    }
    return d;
}

void NodeDumper::dump(Node *n)
{
    NodeDumper dump;
    dump.visitNode(n);
}

void NodeDumper::visitNode(Node *n)
{
    if (!n->isSubtreeEnabled())
        return;
    qDebug() << QString(m_indent * 2, QLatin1Char(' ')) << n;
    NodeVisitor::visitNode(n);
}

void NodeDumper::visitChildren(Node *n)
{
    ++m_indent;
    NodeVisitor::visitChildren(n);
    --m_indent;
}

#endif
