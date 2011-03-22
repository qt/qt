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

#include "qsggeometry.h"
#include <QtGui/QMatrix4x4>

#include <float.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

#define QML_RUNTIME_TESTING

class Renderer;

class Node;
class RootNode;
class GeometryNode;
class TransformNode;
class ClipNode;

class Q_DECLARATIVE_EXPORT Node
{
public:
    enum NodeType {
        BasicNodeType,
        RootNodeType,
        GeometryNodeType,
        TransformNodeType,
        ClipNodeType,
        OpacityNodeType,
        UserNodeType = 1024
    };

    enum DirtyFlag {
        DirtyMatrix                 = 0x0001,
        DirtyClipList               = 0x0002,
        DirtyNodeAdded              = 0x0004,
        DirtyNodeRemoved            = 0x0008,
        DirtyGeometry               = 0x0010,
        DirtyRenderOrder            = 0x0020,
        DirtyMaterial               = 0x0100,
        DirtyOpacity                = 0x0200,
        DirtyAll                    = 0xffff,

        DirtyPropagationMask        = DirtyMatrix
                                      | DirtyClipList
                                      | DirtyNodeAdded
                                      | DirtyOpacity,

    };
    Q_DECLARE_FLAGS(DirtyFlags, DirtyFlag)

    enum Flag {
        OwnedByParent               = 0x0001,
        UsePreprocess               = 0x0002,
        ChildrenDoNotOverloap       = 0x0004,

        // BasicGeometryNode
        OwnsGeometry                = 0x00010000
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

    void clearDirty() { m_flags = 0; }
    void markDirty(DirtyFlags flags);
    DirtyFlags dirtyFlags() const { return m_flags; }

    virtual bool isSubtreeBlocked() const { return false; }

    Flags flags() const { return m_nodeFlags; }
    void setFlag(Flag, bool = true);

    virtual void preprocess() { }

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

    Flags m_nodeFlags;
    DirtyFlags m_flags;
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

    void setGeometry(QSGGeometry *geometry);
    const QSGGeometry *geometry() const { return m_geometry; }
    QSGGeometry *geometry() { return m_geometry; }

    const QMatrix4x4 *matrix() const { return m_matrix; }
    const ClipNode *clipList() const { return m_clip_list; }

private:
    friend class NodeUpdater;
    QSGGeometry *m_geometry;

    int m_reserved_start_index;
    int m_reserved_end_index;

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

    void setOpaqueMaterial(AbstractMaterial *material);
    AbstractMaterial *opaqueMaterial() const { return m_opaque_material; }

    AbstractMaterial *activeMaterial() const;

    virtual NodeType type() const { return GeometryNodeType; }

    void setRenderOrder(int order);
    int renderOrder() const { return m_render_order; }

    void setInheritedOpacity(qreal opacity);
    qreal inheritedOpacity() const { return m_opacity; }

private:
    friend class NodeUpdater;

    int m_render_order;
    AbstractMaterial *m_material;
    AbstractMaterial *m_opaque_material;

    qreal m_opacity;
};

class Q_DECLARATIVE_EXPORT ClipNode : public BasicGeometryNode
{
public:
    ClipNode();
    ~ClipNode();

    virtual NodeType type() const { return ClipNodeType; }

    void setIsRectangular(bool rectHint);
    bool isRectangular() const { return m_is_rectangular; }

    void setClipRect(const QRectF &);
    QRectF clipRect() const { return m_clip_rect; }

private:
    uint m_is_rectangular : 1;
    uint m_reserved : 31;

    QRectF m_clip_rect;
};


class Q_DECLARATIVE_EXPORT TransformNode : public Node
{
public:
    TransformNode();
    ~TransformNode();

    virtual NodeType type() const { return TransformNodeType; }

    void setMatrix(const QMatrix4x4 &matrix);
    const QMatrix4x4 &matrix() const { return m_matrix; }

    void setCombinedMatrix(const QMatrix4x4 &matrix);
    const QMatrix4x4 &combinedMatrix() const { return m_combined_matrix; }

private:
    QMatrix4x4 m_matrix;
    QMatrix4x4 m_combined_matrix;
};


class Q_DECLARATIVE_EXPORT RootNode : public Node
{
public:
    ~RootNode();
    NodeType type() const { return RootNodeType; }

private:
    void notifyNodeChange(Node *node, DirtyFlags flags);

    friend class Renderer;
    friend class Node;
    friend class GeometryNode;

    QList<Renderer *> m_renderers;
};


class Q_DECLARATIVE_EXPORT OpacityNode : public Node
{
public:
    OpacityNode();
    ~OpacityNode();

    void setOpacity(qreal opacity);
    qreal opacity() const { return m_opacity; }

    void setCombinedOpacity(qreal opacity);
    qreal combinedOpacity() const { return m_combined_opacity; }

    virtual Node::NodeType type() const { return OpacityNodeType; }

    bool isSubtreeBlocked() const;


private:
    qreal m_opacity;
    qreal m_combined_opacity;
};

class Q_DECLARATIVE_EXPORT NodeVisitor {
public:
    virtual ~NodeVisitor();

protected:
    virtual void enterTransformNode(TransformNode *) {}
    virtual void leaveTransformNode(TransformNode *) {}
    virtual void enterClipNode(ClipNode *) {}
    virtual void leaveClipNode(ClipNode *) {}
    virtual void enterGeometryNode(GeometryNode *) {}
    virtual void leaveGeometryNode(GeometryNode *) {}
    virtual void enterOpacityNode(OpacityNode *) {}
    virtual void leaveOpacityNode(OpacityNode *) {}
    virtual void visitNode(Node *n);
    virtual void visitChildren(Node *n);
};

#ifndef QT_NO_DEBUG_STREAM
Q_DECLARATIVE_EXPORT QDebug operator<<(QDebug, const Node *n);
Q_DECLARATIVE_EXPORT QDebug operator<<(QDebug, const GeometryNode *n);
Q_DECLARATIVE_EXPORT QDebug operator<<(QDebug, const TransformNode *n);
Q_DECLARATIVE_EXPORT QDebug operator<<(QDebug, const OpacityNode *n);
Q_DECLARATIVE_EXPORT QDebug operator<<(QDebug, const RootNode *n);

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

QT_END_NAMESPACE

QT_END_HEADER

#endif // NODE_H
