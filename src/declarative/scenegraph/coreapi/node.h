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

#ifndef NODE_H
#define NODE_H

#include "geometry.h"
#include <QtGui/QMatrix4x4>

#include <float.h>

#define QML_RUNTIME_TESTING

class Renderer;

class Node;
class RootNode;
class GeometryNode;
class TransformNode;
class ClipNode;

class NodeData
{
public:
    virtual ~NodeData() { }
};

class Q_DECLARATIVE_EXPORT Node
{
public:
    enum NodeType {
        BasicNodeType,
        RootNodeType,
        GeometryNodeType,
        TransformNodeType,
        ClipNodeType,
        UserNodeType = 1024
    };

    enum DirtyFlag {
        DirtyMatrix                 = 0x0001,
        DirtyClipList               = 0x0002,
        DirtyNodeAdded              = 0x0004,
        DirtyNodeRemoved            = 0x0008,
        DirtyGeometry               = 0x0010,
        DirtyRenderOrder            = 0x0020,
        DirtySubtreeEnabled         = 0x0040,
        DirtyBoundingRect           = 0x0080,
        DirtyMaterial               = 0x0100,
        DirtyAll                    = 0xffff,

        DirtyPropagationMask        = DirtyMatrix | DirtyClipList | DirtyNodeAdded | DirtySubtreeEnabled | DirtyBoundingRect,

//        DirtyMatrixSubtree          = DirtyMatrix << 16,
//        DirtyClipListSubtree        = DirtyClipList << 16,
//        DirtyNodeAddedSubtree       = DirtyNodeAdded << 16,
//        DirtyNodeRemovedSubtree     = DirtyNodeRemoved << 16,
//        DirtyGeometrySubtree        = DirtyGeometry << 16,
//        DirtyRenderOrderSubtree     = DirtyRenderOrder << 16,
//        DirtyAllSubtree             = 0xffff0000
    };
    Q_DECLARE_FLAGS(DirtyFlags, DirtyFlag)

    enum Flag {
        OwnedByParent               = 0x0001,
        UsePreprocess               = 0x0002,
        ChildrenDoNotOverloap       = 0x0004,
        ClipIsRectangular           = 0x0008
    };
    Q_DECLARE_FLAGS(Flags, Flag)

    enum NodeSubType {
        DefaultNodeSubType,

        TextNodeSubType,
        GlyphNodeSubType,
        SolidRectNodeSubType,
        PixmapNodeSubType,
        QuadNodeSubType,
        EffectSubTreeNodeSubType,
        TextureNodeInterfaceSubType,
    };

    Node();
    virtual ~Node();

    Node *parent() const { return m_parent; }

    void removeChildNode(Node *node);
    void prependChildNode(Node *node);
    void appendChildNode(Node *node);
    void insertChildNodeBefore(Node *node, Node *before);
    void insertChildNodeAfter(Node *node, Node *after);

    int childCount() const { return m_children.size(); }
    Node *childAtIndex(int i) const { return m_children.at(i); }

    virtual NodeType type() const { return BasicNodeType; }

    virtual NodeSubType subType() const { return DefaultNodeSubType; }

    virtual void setSubtreeRenderOrder(int order);

    void setNodeData(void *key, NodeData *data) {
        m_data[key] = data;
    }

    NodeData *nodeData(void *key) const {
        return m_data[key];
    }

    RootNode *root() const;

    void clearDirty() { m_flags = 0; }
    void markDirty(DirtyFlags flags);
    DirtyFlags dirtyFlags() const { return m_flags; }

    void updateDirtyStates();

    bool isSubtreeEnabled() const { return m_subtree_enabled; }
    void setSubtreeEnabled(bool enabled);

    Flags flags() const { return m_nodeFlags; }
    void setFlag(Flag, bool = true);

    virtual void preprocess() { }

    virtual QRectF subtreeBoundingRect() const;

#ifdef QML_RUNTIME_TESTING
    QString description;
#endif

protected:
    // When a node is destroyed, it will detach from the scene graph and the renderer will be
    // notified about the change. If the node is detached in the base node's destructor, the
    // renderer can't check what type the node originally was because the node's type() method is
    // virtual and will return the base node type. The renderer might therefore react incorrectly
    // to the change. There are a few of ways I can think of to solve the problem:
    // - The renderer must take into account that the notify method might be called from a node's
    //   destructor.
    // - The node can have a type property that is set in the constructor.
    // - All the node destructors must call a common destroy method.
    // I choose the third option since that will allow the renderer to treat the nodes as their
    // proper types.

    void destroy();

private:
    Node *m_parent;
    QList<Node *> m_children;
    QHash<void *, NodeData *> m_data;

    Flags m_nodeFlags;
    DirtyFlags m_flags;
    uint m_updateFlags;
    bool m_subtree_enabled;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(Node::DirtyFlags);
Q_DECLARE_OPERATORS_FOR_FLAGS(Node::Flags);

class Q_DECLARATIVE_EXPORT BasicGeometryNode : public Node
{
public:
//    enum  UsagePattern {
//        Static,
//        Dynamic,
//        Stream
//    };
//    void setUsagePattern(UsagePattern pattern);
//    UsagePattern usagePattern() const { return m_pattern; }

    BasicGeometryNode();
    ~BasicGeometryNode();

    void setGeometry(Geometry *geometry, int firstIndex = 0, int endIndex = -1);
    void updateGeometryDescription(const QVector<QSGAttributeDescription> &description, GLenum indexType);
    Geometry *geometry() const { return m_geometry; }
    int firstIndex() const { return m_first_index; }
    void setFirstIndex(int index);
    int endIndex() const { return m_end_index; }
    void setEndIndex(int index);
    QPair<int, int> indexRange() const; // If end index < 0, the upper bound will be set to the vertex or index count.

    void setBoundingRect(const QRectF &bounds);
    QRectF boundingRect() const { return m_bounding_rect; }

    const QMatrix4x4 *matrix() const { return m_matrix; }
    const ClipNode *clipList() const { return m_clip_list; }

private:
    friend class NodeUpdater;
    Geometry *m_geometry;
    int m_first_index;
    int m_end_index;
    QRectF m_bounding_rect;

    const QMatrix4x4 *m_matrix;
    const ClipNode *m_clip_list;

//    UsagePattern m_pattern;
};

class AbstractMaterial;

class Q_DECLARATIVE_EXPORT GeometryNode : public BasicGeometryNode
{
public:
    GeometryNode();
    ~GeometryNode();

    void setMaterial(AbstractMaterial *material);
    AbstractMaterial *material() const { return m_material; }

    virtual NodeType type() const { return GeometryNodeType; }
    virtual QRectF subtreeBoundingRect() const;

    void setRenderOrder(int order);
    int renderOrder() const { return m_render_order; }

    virtual void setSubtreeRenderOrder(int order);

    bool isEnabled() const { return m_enabled; }

private:
    friend class NodeUpdater;

    int m_render_order;
    AbstractMaterial *m_material;
    bool m_enabled;
};

class Q_DECLARATIVE_EXPORT ClipNode : public BasicGeometryNode
{
public:
    ClipNode();
    ~ClipNode();

    virtual NodeType type() const { return ClipNodeType; }
    virtual QRectF subtreeBoundingRect() const;

private:
};


class Q_DECLARATIVE_EXPORT TransformNode : public Node
{
public:
    TransformNode();
    ~TransformNode();

    virtual NodeType type() const { return TransformNodeType; }
    virtual QRectF subtreeBoundingRect() const;

    void setMatrix(const QMatrix4x4 &matrix);
    const QMatrix4x4 matrix() const { return m_matrix; }

    void translate(qreal dx, qreal dy, qreal dz=0);
    void scale(qreal factor);
    void scale(qreal x, qreal y, qreal z = 1);
    void rotate(qreal angle, qreal x, qreal y, qreal z);

    void setZ(qreal z);

private:
    friend class NodeUpdater;

    QMatrix4x4 m_matrix;
    QMatrix4x4 m_combined_matrix;
};


class Q_DECLARATIVE_EXPORT RootNode : public Node
{
public:
    ~RootNode();
    NodeType type() const { return RootNodeType; }

    void addRenderer(Renderer *r) { m_renderers << r; }
    void removeRenderer(Renderer *r);

    void updateDirtyStates();

private:
    void notifyNodeChange(Node *node, DirtyFlags flags);
    void notifyMaterialChange(GeometryNode *node, AbstractMaterial *from, AbstractMaterial *to);

    friend class Node;
    friend class GeometryNode;

    QList<Renderer *> m_renderers;
};

class NodeVisitor {
public:
    virtual ~NodeVisitor();

    virtual void enterTransformNode(TransformNode *) {}
    virtual void leaveTransformNode(TransformNode *) {}
    virtual void enterClipNode(ClipNode *) {}
    virtual void leaveClipNode(ClipNode *) {}
    virtual void enterGeometryNode(GeometryNode *) {}
    virtual void leaveGeometryNode(GeometryNode *) {}

    virtual void visitNode(Node *n);

    virtual void visitChildren(Node *n);
};

#ifndef QT_NO_DEBUG_STREAM
Q_DECLARATIVE_EXPORT QDebug operator<<(QDebug, const Node *n);
Q_DECLARATIVE_EXPORT QDebug operator<<(QDebug, const GeometryNode *n);
Q_DECLARATIVE_EXPORT QDebug operator<<(QDebug, const TransformNode *n);

class NodeDumper : public NodeVisitor {

public:
    static void dump(Node *n);

    NodeDumper() : m_indent(0) {}
    void visitNode(Node *n);
    void visitChildren(Node *n);

private:
    int m_indent;
};

#endif

#endif // NODE_H
