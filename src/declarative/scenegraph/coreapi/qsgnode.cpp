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

#include "qsgnode.h"
#include "qsgrenderer_p.h"
#include "qsgnodeupdater_p.h"
#include "qsgmaterial.h"

#include "limits.h"

QT_BEGIN_NAMESPACE

QSGNode::QSGNode()
    : m_parent(0)
    , m_nodeFlags(OwnedByParent)
    , m_flags(0)
{
}

QSGNode::~QSGNode()
{
    destroy();
}


/*!
    \fn bool QSGNode::isSubtreeBlocked() const

    Returns whether this node and its subtree is available for use.

    Blocked subtrees will not get their dirty states updated and they
    will not be rendered.

    The OpacityNode will return a blocked subtree when accumulated opacity
    is 0, for instance.
 */


void QSGNode::destroy()
{
    if (m_parent) {
        m_parent->removeChildNode(this);
        Q_ASSERT(m_parent == 0);
    }
    for (int ii = m_children.count() - 1; ii >= 0; --ii) {
        QSGNode *child = m_children.at(ii);
        removeChildNode(child);
        Q_ASSERT(child->m_parent == 0);
        if (child->flags() & OwnedByParent)
            delete child;
    }
    Q_ASSERT(m_children.isEmpty());
}

void QSGNode::prependChildNode(QSGNode *node)
{
    Q_ASSERT_X(!m_children.contains(node), "QSGNode::prependChildNode", "QSGNode is already a child!");
    Q_ASSERT_X(!node->m_parent, "QSGNode::prependChildNode", "QSGNode already has a parent");

#ifndef QT_NO_DEBUG
    if (node->type() == QSGNode::GeometryNodeType) {
        QSGGeometryNode *g = static_cast<QSGGeometryNode *>(node);
        Q_ASSERT_X(g->material(), "QSGNode::prependChildNode", "QSGGeometryNode is missing material");
        Q_ASSERT_X(g->geometry(), "QSGNode::prependChildNode", "QSGGeometryNode is missing geometry");
    }
#endif

    m_children.prepend(node);
    node->m_parent = this;

    node->markDirty(DirtyNodeAdded);
}

void QSGNode::appendChildNode(QSGNode *node)
{
    Q_ASSERT_X(!m_children.contains(node), "QSGNode::appendChildNode", "QSGNode is already a child!");
    Q_ASSERT_X(!node->m_parent, "QSGNode::appendChildNode", "QSGNode already has a parent");

#ifndef QT_NO_DEBUG
    if (node->type() == QSGNode::GeometryNodeType) {
        QSGGeometryNode *g = static_cast<QSGGeometryNode *>(node);
        Q_ASSERT_X(g->material(), "QSGNode::appendChildNode", "QSGGeometryNode is missing material");
        Q_ASSERT_X(g->geometry(), "QSGNode::appendChildNode", "QSGGeometryNode is missing geometry");
    }
#endif

    m_children.append(node);
    node->m_parent = this;

    node->markDirty(DirtyNodeAdded);
}

void QSGNode::insertChildNodeBefore(QSGNode *node, QSGNode *before)
{
    Q_ASSERT_X(!m_children.contains(node), "QSGNode::insertChildNodeBefore", "QSGNode is already a child!");
    Q_ASSERT_X(!node->m_parent, "QSGNode::insertChildNodeBefore", "QSGNode already has a parent");
    Q_ASSERT_X(node->type() != RootNodeType, "QSGNode::insertChildNodeBefore", "RootNodes cannot be children of other nodes");

#ifndef QT_NO_DEBUG
    if (node->type() == QSGNode::GeometryNodeType) {
        QSGGeometryNode *g = static_cast<QSGGeometryNode *>(node);
        Q_ASSERT_X(g->material(), "QSGNode::insertChildNodeBefore", "QSGGeometryNode is missing material");
        Q_ASSERT_X(g->geometry(), "QSGNode::insertChildNodeBefore", "QSGGeometryNode is missing geometry");
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

void QSGNode::insertChildNodeAfter(QSGNode *node, QSGNode *after)
{
    Q_ASSERT_X(!m_children.contains(node), "QSGNode::insertChildNodeAfter", "QSGNode is already a child!");
    Q_ASSERT_X(!node->m_parent, "QSGNode::insertChildNodeAfter", "QSGNode already has a parent");
    Q_ASSERT_X(node->type() != RootNodeType, "QSGNode::insertChildNodeAfter", "RootNodes cannot be children of other nodes");

#ifndef QT_NO_DEBUG
    if (node->type() == QSGNode::GeometryNodeType) {
        QSGGeometryNode *g = static_cast<QSGGeometryNode *>(node);
        Q_ASSERT_X(g->material(), "QSGNode::insertChildNodeAfter", "QSGGeometryNode is missing material");
        Q_ASSERT_X(g->geometry(), "QSGNode::insertChildNodeAfter", "QSGGeometryNode is missing geometry");
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

void QSGNode::removeChildNode(QSGNode *node)
{
    Q_ASSERT(m_children.contains(node));
    Q_ASSERT(node->parent() == this);

    m_children.removeOne(node);

    node->markDirty(DirtyNodeRemoved);
    node->m_parent = 0;
}


void QSGNode::setFlag(Flag f, bool enabled)
{
    if (enabled)
        m_nodeFlags |= f;
    else
        m_nodeFlags &= ~f;
}

void QSGNode::markDirty(DirtyFlags flags)
{
    m_flags |= (flags & DirtyPropagationMask);

    DirtyFlags subtreeFlags = DirtyFlags((flags & DirtyPropagationMask) << 16);
    QSGNode *p = m_parent;
    while (p) {
        p->m_flags |= subtreeFlags;
        if (p->type() == RootNodeType)
            static_cast<QSGRootNode *>(p)->notifyNodeChange(this, flags);
        p = p->m_parent;
    }

}

QSGBasicGeometryNode::QSGBasicGeometryNode()
    : m_geometry(0)
    , m_matrix(0)
    , m_clip_list(0)
{
}

QSGBasicGeometryNode::~QSGBasicGeometryNode()
{
    destroy();
    if (flags() & OwnsGeometry)
        delete m_geometry;
}

void QSGBasicGeometryNode::setGeometry(QSGGeometry *geometry)
{
    if (flags() & OwnsGeometry)
        delete m_geometry;
    m_geometry = geometry;
    markDirty(DirtyGeometry);
}


QSGGeometryNode::QSGGeometryNode()
    : m_render_order(0)
    , m_material(0)
    , m_opaque_material(0)
    , m_opacity(1)
{
}

QSGGeometryNode::~QSGGeometryNode()
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

    \internal
  */
void QSGGeometryNode::setRenderOrder(int order)
{
    m_render_order = order;
}



/*!
    Sets the material of this geometry node to \a material.

    GeometryNodes must have a material before they can be added to the
    scene graph.
 */
void QSGGeometryNode::setMaterial(QSGMaterial *material)
{
    m_material = material;
    markDirty(DirtyMaterial);
}



/*!
    Sets the opaque material of this geometry to \a material.

    The opaque material will be preferred by the renderer over the
    default material, as returned by the material() function, if
    it is not null and the geometry item has an inherited opacity of
    1.

    The opaqueness refers to scene graph opacity, the material is still
    allowed to set QSGMaterial::Blending to true and draw transparent
    pixels.
 */
void QSGGeometryNode::setOpaqueMaterial(QSGMaterial *material)
{
    m_opaque_material = material;
    markDirty(DirtyMaterial);
}



/*!
    Returns the material which should currently be used for geometry node.

    If the inherited opacity of the node is 1 and there is an opaque material
    set on this node, it will be returned; otherwise, the default material
    will be returned.

    \warning This function requires the scene graph above this item to be
    completely free of dirty states, so it can only be called during rendering

    \internal

    \sa setMaterial, setOpaqueMaterial
 */
QSGMaterial *QSGGeometryNode::activeMaterial() const
{
    Q_ASSERT_X(dirtyFlags() == 0, "QSGGeometryNode::activeMaterial()", "function assumes that all dirty states are cleaned up");
    if (m_opaque_material && m_opacity > 0.999)
        return m_opaque_material;
    return m_material;
}


/*!
    Sets the inherited opacity of this geometry to \a opacity.

    This function is meant to be called by the node preprocessing
    prior to rendering the tree, so it will not mark the tree as
    dirty.

    \internal
  */
void QSGGeometryNode::setInheritedOpacity(qreal opacity)
{
    Q_ASSERT(opacity >= 0 && opacity <= 1);
    m_opacity = opacity;
}



QSGClipNode::QSGClipNode()
{
}

QSGClipNode::~QSGClipNode()
{
    destroy();
}

/*!
    Sets whether this clip node has a rectangular clip to \a rectHint.
 */
void QSGClipNode::setIsRectangular(bool rectHint)
{
    m_is_rectangular = rectHint;
}


/*!
    Sets the clip rect of this clip node to \a rect.

    When a rectangular clip is set in combination with setIsRectangular
    the renderer may in some cases use a more optimal clip method.
 */
void QSGClipNode::setClipRect(const QRectF &rect)
{
    m_clip_rect = rect;
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


/*!
    Sets the combined matrix of this matrix to \a transform.

    This function is meant to be called by the node preprocessing
    prior to rendering the tree, so it will not mark the tree as
    dirty.

    \internal
  */
void TransformNode::setCombinedMatrix(const QMatrix4x4 &matrix)
{
    m_combined_matrix = matrix;
}



QSGRootNode::~QSGRootNode()
{
    while (!m_renderers.isEmpty())
        m_renderers.last()->setRootNode(0);
    destroy();
}


void QSGRootNode::notifyNodeChange(QSGNode *node, DirtyFlags flags)
{
    for (int i=0; i<m_renderers.size(); ++i) {
        m_renderers.at(i)->nodeChanged(node, flags);
    }
}

/*!
    Constructs an opacity node with a default opacity of 1.

    Opacity accumulate downwards in the scene graph so a node with two
    OpacityNode instances above it, both with opacity of 0.5, will have
    effective opacity of 0.25.

    The default opacity of nodes is 1.
  */
OpacityNode::OpacityNode()
    : m_opacity(1)
    , m_combined_opacity(1)
{
}


OpacityNode::~OpacityNode()
{
    destroy();
}


/*!
    Sets the opacity of this node to \a opacity.

    Before rendering the graph, the renderer will do an update pass
    over the subtree to propegate the opacity to its children.

    The value will be bounded to the range 0 to 1.
 */
void OpacityNode::setOpacity(qreal opacity)
{
    opacity = qBound<qreal>(0, opacity, 1);
    if (m_opacity == opacity)
        return;
    m_opacity = opacity;
    markDirty(DirtyOpacity);
}


/*!
    Sets the combined opacity of this node to \a opacity.

    This function is meant to be called by the node preprocessing
    prior to rendering the tree, so it will not mark the tree as
    dirty.

    \internal
 */
void OpacityNode::setCombinedOpacity(qreal opacity)
{
    m_combined_opacity = opacity;
}


bool OpacityNode::isSubtreeBlocked() const
{
    return m_combined_opacity < 0.001;
}


QSGNodeVisitor::~QSGNodeVisitor()
{

}


void QSGNodeVisitor::visitNode(QSGNode *n)
{
    switch (n->type()) {
    case QSGNode::TransformNodeType: {
        TransformNode *t = static_cast<TransformNode *>(n);
        enterTransformNode(t);
        visitChildren(t);
        leaveTransformNode(t);
        break; }
    case QSGNode::GeometryNodeType: {
        QSGGeometryNode *g = static_cast<QSGGeometryNode *>(n);
        enterGeometryNode(g);
        visitChildren(g);
        leaveGeometryNode(g);
        break; }
    case QSGNode::ClipNodeType: {
        QSGClipNode *c = static_cast<QSGClipNode *>(n);
        enterClipNode(c);
        visitChildren(c);
        leaveClipNode(c);
        break; }
    case QSGNode::OpacityNodeType: {
        OpacityNode *o = static_cast<OpacityNode *>(n);
        enterOpacityNode(o);
        visitChildren(o);
        leaveOpacityNode(o);
        break; }
    default:
        visitChildren(n);
        break;
    }
}

void QSGNodeVisitor::visitChildren(QSGNode *n)
{
    int count = n->childCount();
    for (int i=0; i<count; ++i) {
        visitNode(n->childAtIndex(i));
    }
}



#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug d, const QSGGeometryNode *n)
{
    if (!n) {
        d << "QSGGeometryNode(null)";
        return d;
    }
    d << "QSGGeometryNode(" << hex << (void *) n << dec;

    const QSGGeometry *g = n->geometry();

    if (!g) {
        d << "no geometry";
    } else {

        switch (g->drawingMode()) {
        case GL_TRIANGLE_STRIP: d << "strip"; break;
        case GL_TRIANGLE_FAN: d << "fan"; break;
        case GL_TRIANGLES: d << "triangles"; break;
        default: break;
        }

         d << g->vertexCount();

         if (g->attributeCount() > 0 && g->attributes()->type == GL_FLOAT) {
             float x1 = 1e10, x2 = -1e10, y1=1e10, y2=-1e10;
             int stride = g->stride();
             for (int i = 0; i < g->vertexCount(); ++i) {
                 float x = ((float *)((char *)const_cast<QSGGeometry *>(g)->vertexData() + i * stride))[0];
                 float y = ((float *)((char *)const_cast<QSGGeometry *>(g)->vertexData() + i * stride))[1];

                 x1 = qMin(x1, x);
                 x2 = qMax(x2, x);
                 y1 = qMin(y1, y);
                 y2 = qMax(y2, y);
             }

             d << "x1=" << x1 << "y1=" << y1 << "x2=" << x2 << "y2=" << y2;
         }
    }

    d << "order=" << n->renderOrder();
    if (n->material())
        d << "effect=" << n->material() << "type=" << n->material()->type();


    d << ")";
#ifdef QML_RUNTIME_TESTING
    d << n->description;
#endif
    d << "dirty=" << hex << (int) n->dirtyFlags() << dec;
    return d;
}

QDebug operator<<(QDebug d, const QSGClipNode *n)
{
    if (!n) {
        d << "QSGClipNode(null)";
        return d;
    }
    d << "QSGClipNode(" << hex << (void *) n << dec;

    if (n->childCount())
        d << "children=" << n->childCount();

    d << "is rect?" << (n->isRectangular() ? "yes" : "no");

    d << ")";
#ifdef QML_RUNTIME_TESTING
    d << n->description;
#endif
    d << "dirty=" << hex << (int) n->dirtyFlags() << dec << (n->isSubtreeBlocked() ? "*BLOCKED*" : "");
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
    d << "dirty=" << hex << (int) n->dirtyFlags() << dec << (n->isSubtreeBlocked() ? "*BLOCKED*" : "");
    d << ")";
    return d;
}

QDebug operator<<(QDebug d, const OpacityNode *n)
{
    if (!n) {
        d << "OpacityNode(null)";
        return d;
    }
    d << "OpacityNode(";
    d << hex << (void *) n << dec;
    d << "opacity=" << n->opacity()
      << "combined=" << n->combinedOpacity()
      << (n->isSubtreeBlocked() ? "*BLOCKED*" : "");
#ifdef QML_RUNTIME_TESTING
    d << n->description;
#endif
    d << "dirty=" << hex << (int) n->dirtyFlags() << dec;
    d << ")";
    return d;
}


QDebug operator<<(QDebug d, const QSGRootNode *n)
{
    if (!n) {
        d << "QSGRootNode(null)";
        return d;
    }
    d << "QSGRootNode" << hex << (void *) n << "dirty=" << (int) n->dirtyFlags() << dec
      << (n->isSubtreeBlocked() ? "*BLOCKED*" : "");
#ifdef QML_RUNTIME_TESTING
    d << n->description;
#endif
    d << ")";
    return d;
}



QDebug operator<<(QDebug d, const QSGNode *n)
{
    if (!n) {
        d << "QSGNode(null)";
        return d;
    }
    switch (n->type()) {
    case QSGNode::GeometryNodeType:
        d << static_cast<const QSGGeometryNode *>(n);
        break;
    case QSGNode::TransformNodeType:
        d << static_cast<const TransformNode *>(n);
        break;
    case QSGNode::ClipNodeType:
        d << static_cast<const QSGClipNode *>(n);
        break;
    case QSGNode::RootNodeType:
        d << static_cast<const QSGRootNode *>(n);
        break;
    case QSGNode::OpacityNodeType:
        d << static_cast<const OpacityNode *>(n);
        break;
    default:
        d << "QSGNode(" << hex << (void *) n << dec
          << "dirty=" << hex << (int) n->dirtyFlags() << dec
          << (n->isSubtreeBlocked() ? "*BLOCKED*" : "");
#ifdef QML_RUNTIME_TESTING
        d << n->description;
#endif
        d << ")";
        break;
    }
    return d;
}

void QSGNodeDumper::dump(QSGNode *n)
{
    QSGNodeDumper dump;
    dump.visitNode(n);
}

void QSGNodeDumper::visitNode(QSGNode *n)
{
    if (n->isSubtreeBlocked())
        return;
    qDebug() << QString(m_indent * 2, QLatin1Char(' ')) << n;
    QSGNodeVisitor::visitNode(n);
}

void QSGNodeDumper::visitChildren(QSGNode *n)
{
    ++m_indent;
    QSGNodeVisitor::visitChildren(n);
    --m_indent;
}

#endif

QT_END_NAMESPACE
