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

#ifndef RENDERER_H
#define RENDERER_H

#include <qset.h>
#include <qhash.h>

#include "qsgmatrix4x4stack.h"

#include <qsgarray.h>

#include <qglfunctions.h>
#include <qglshaderprogram.h>

#include "node.h"
#include "qsgtexturemanager.h"

class AbstractMaterialShader;
struct AbstractMaterialType;
class QGLFramebufferObject;
class TextureReference;

class Q_DECLARATIVE_EXPORT Bindable
{
public:
    virtual ~Bindable() { }
    virtual void bind() const = 0;
    virtual void clear() const;
    virtual void reactivate() const;
};

class BindableFbo : public Bindable
{
public:
    BindableFbo(QGLContext *ctx, QGLFramebufferObject *fbo);
    virtual void bind() const;
private:
    QGLContext *m_ctx;
    QGLFramebufferObject *m_fbo;
};

class NodeUpdater;

class Q_DECLARATIVE_EXPORT Renderer : public QObject, public QGLFunctions
{
    Q_OBJECT
public:
    enum Update
    {
        UpdateMatrices              = 0x0001,
        UpdateOpacity               = 0x0002
    };
    Q_DECLARE_FLAGS(Updates, Update)

    enum ClipType
    {
        NoClip,
        ScissorClip,
        StencilClip
    };

    Renderer();
    virtual ~Renderer();

    void setRootNode(RootNode *node);
    RootNode *rootNode() const { return m_root_node; }

    void setDeviceRect(const QRect &rect) { m_device_rect = rect; }
    inline void setDeviceRect(const QSize &size) { setDeviceRect(QRect(QPoint(), size)); }
    QRect deviceRect() const { return m_device_rect; }

    void setViewportRect(const QRect &rect) { m_viewport_rect = rect; }
    inline void setViewportRect(const QSize &size) { setViewportRect(QRect(QPoint(), size)); }
    QRect viewportRect() const { return m_viewport_rect; }

    QSGMatrix4x4Stack &projectionMatrix() { return m_projectionMatrix; }
    QSGMatrix4x4Stack &modelViewMatrix() { return m_modelViewMatrix; }

    void setProjectMatrixToDeviceRect();
    void setProjectMatrixToRect(const QRectF &rect);
    void setProjectMatrix(const QMatrix4x4 &matrix);
    QMatrix4x4 projectMatrix() const { return m_projection_matrix; }
    bool isMirrored() const { return m_mirrored; }

    QMatrix4x4 combinedMatrix() const { return m_projectionMatrix.top() * m_modelViewMatrix.top(); }

    // ### gunnar: move into RenderState along with combined matrix and update flags.
    qreal renderOpacity() const { return m_render_opacity; }

    void setClearColor(const QColor &color);

    void setTexture(int unit, const QSGTextureRef &texture);
    void setTexture(const QSGTextureRef &texture) { setTexture(0, texture); }

    void renderScene();
    void renderScene(const Bindable &bindable);
    virtual void nodeChanged(Node *node, Node::DirtyFlags flags);
    virtual void materialChanged(GeometryNode *node, AbstractMaterial *from, AbstractMaterial *to);

    NodeUpdater *nodeUpdater() const;
    void setNodeUpdater(NodeUpdater *updater);

signals:
    void sceneGraphChanged(); // Add, remove, ChangeFlags changes...

protected:
    virtual void render() = 0;
    Renderer::ClipType updateStencilClip(const ClipNode *clip);

    const Bindable *bindable() const { return m_bindable; }

    AbstractMaterialShader *prepareMaterial(AbstractMaterial *material);
    virtual void preprocess();

    void addNodesToPreprocess(Node *node);
    void removeNodesToPreprocess(Node *node);

    QColor m_clear_color;
    QSGMatrix4x4Stack m_projectionMatrix;
    QSGMatrix4x4Stack m_modelViewMatrix;
    qreal m_render_opacity;

private:
    RootNode *m_root_node;
    NodeUpdater *m_node_updater;

    QRect m_device_rect;
    QRect m_viewport_rect;

    QHash<AbstractMaterialType *, AbstractMaterialShader *> m_materials;
    QSet<Node *> m_nodes_to_preprocess;

    QMatrix4x4 m_projection_matrix;
    QGLShaderProgram m_clip_program;
    int m_clip_matrix_id;

    bool m_changed_emitted;
    bool m_mirrored;

    const Bindable *m_bindable;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(Renderer::Updates)

#endif // RENDERER_H
